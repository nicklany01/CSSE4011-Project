import asyncio
import copy

from random import randint
from datetime import datetime
import time

from bleak import BleakScanner, BleakClient

from pet_app_helpers import PET_WFC_DEMO_CMDS, SPRITE, PET_PKT_ID, PET_BLE_ADV_POS, \
	PetJournalEntry, PetJournal


BLE_SIENNA_MF_ID = 0x6943
SIENNA_MASTER_PEX = 0x4369

JOURNAL_FINISHED_MAGIC_NUM = 96
JOURNAL_REQUEST_MAGIC_NUM = 127

BLE_UUID_SRV_PPY = "4A259CE4-4369-4153-AD28-B8D61B4F447A"
BLE_UUID_CHR_PPY_RX = "4A259CE4-4770-4153-AD28-B8D61B4F447A"
BLE_UUID_CHR_PPY_TX = "4A259CE4-4771-4153-AD28-B8D61B4F447A"

BLE_UUID_SRV_PEX = "4A259CE4-CAFE-4153-AD28-B8D61B4F447A"
BLE_UUID_CHR_PEX_RX = "4A259CE4-4772-4153-AD28-B8D61B4F447A"
BLE_UUID_CHR_PEX_TX = "4A259CE4-4773-4153-AD28-B8D61B4F447A"

BLE_UUID_SRV_WFC = "4A259CE4-FEED-4153-AD28-B8D61B4F447A"
BLE_UUID_CHR_WFC_RX = "4A259CE4-4774-4153-AD28-B8D61B4F447A"
BLE_UUID_CHR_WFC_TX = "4A259CE4-4775-4153-AD28-B8D61B4F447A"

GAME_EVT_PPY_RX_PERSONALITY = None
GAME_EVT_PEX_RX_STATE = None
GAME_EVT_PEX_RX_JOURNAL = None

GAME_EVT_WFC_RX_RTC = None

GAME_RX_JOURNAL = {}

async def pet_ble_init():
	global GAME_EVT_PPY_RX_PERSONALITY, GAME_EVT_PEX_RX_STATE, \
		GAME_EVT_PEX_RX_JOURNAL, GAME_EVT_WFC_RX_RTC

	GAME_EVT_PPY_RX_PERSONALITY = asyncio.Event()
	GAME_EVT_PEX_RX_STATE = asyncio.Event()
	GAME_EVT_PEX_RX_JOURNAL = asyncio.Event()

	GAME_EVT_WFC_RX_RTC = asyncio.Event()

def e_u16(value, buff):
	buff.extend(value.to_bytes(2, "big"))

def d_u16(buff, pos):
	return (buff[pos] << 8) | (buff[pos + 1] & 0xFF)

class PetWFCDemoCmdPkt:

	def __init__(self):
		self.cmd_id = PET_WFC_DEMO_CMDS.CHANGE_SCENE
		self.cmd_arg = 0

	def serialize(self):

		tx_bytes = bytearray()
		tx_bytes.append(PET_PKT_ID.WFC_DEMO_COMMAND)

		tx_bytes.append(self.cmd_id)
		e_u16(self.cmd_arg, tx_bytes)
		
		return tx_bytes

	def __str__(self):
		return f"cmd_id: {self.cmd_id} cmd_arg: {self.cmd_arg}"

	# no deserialize needed, WFC is one way

class PetWFCRtcPkt:
	def __init__(self):

		time = datetime.now()

		self.secs = time.second
		self.mins = time.minute
		self.hrs = time.hour

		self.day = time.day
		self.month = time.month
		# keep in line w zephyr API
		# pet will last 130 more years as uint8...
		self.year = time.year - 1900

	def serialize(self):

		tx_bytes = bytearray()
		tx_bytes.append(PET_PKT_ID.WFC_RTC_UPDATE)

		tx_bytes.append(self.secs)
		tx_bytes.append(self.mins)
		tx_bytes.append(self.hrs)

		tx_bytes.append(self.day)
		tx_bytes.append(self.month)
		tx_bytes.append(self.year)

		return tx_bytes

	def deserialize(self, rx_bytes):

		barray = rx_bytes

		if not isinstance(barray, bytearray):
			barray = bytearray(rx_bytes)

		offset = 1

		self.secs = barray[offset]
		offset += 1
		self.mins = barray[offset]
		offset += 1
		self.hrs = barray[offset]
		offset += 1

		self.day = barray[offset]
		offset += 1
		self.month = barray[offset]
		offset += 1
		self.year = barray[offset]
		offset += 1

	def to_date(self):

		return datetime(year=self.year + 1900, month=self.month,
			day=self.day, hour=self.hrs, minute=self.mins, second=self.secs)

	def __str__(self):

		return f"{self.hrs}:{self.mins}:{self.secs} {self.day}/{self.month}/{self.year}"

	# no deserialize needed, WFC is one way

class PetWFCWeatherPkt:
	def __init__(self):

		self.temp_c = 0
		self.weather = 0

	def serialize(self):

		tx_bytes = bytearray()
		tx_bytes.append(PET_PKT_ID.WFC_WEATHER_UPDATE)

		tx_bytes.append(self.temp_c)
		tx_bytes.append(self.weather)

		return tx_bytes

	def __str__(self):

		return f"temp_c: {self.temp_c} weather: {self.weather}"

	# no deserialize needed, WFC is one way

class PetPPYPersonalityPkt:
	def __init__(self):

		self.pex_id = SIENNA_MASTER_PEX
		self.sprite = SPRITE.CHERRY

		self.fav_scene = 0
		self.fav_weather = 0
		self.fav_time = 0
		self.fav_temp = 0
		self.fav_food = 0
		self.fav_drink = 0

		self.weights = [0 for i in range(0, 9)]

	def randomize(self):

		self.weights = [randint(0, 5) for i in range(0, 9)]

	def serialize(self):

		tx_bytes = bytearray()
		tx_bytes.append(PET_PKT_ID.PPY_PERSONALITY)

		e_u16(self.pex_id, tx_bytes)

		tx_bytes.append(self.sprite)
		tx_bytes.append(self.fav_scene)
		tx_bytes.append(self.fav_weather)
		tx_bytes.append(self.fav_time)
		tx_bytes.append(self.fav_temp)

		tx_bytes.append(self.fav_food)
		tx_bytes.append(self.fav_drink)

		for i in self.weights:
			tx_bytes.append(i)

		return tx_bytes

	def deserialize(self, rx_bytes):

		barray = rx_bytes

		if not isinstance(barray, bytearray):
			barray = bytearray(rx_bytes)

		offset = 1
		self.pex_id = d_u16(barray, offset)
		offset += 2

		self.sprite = barray[offset]
		offset += 1
		self.fav_scene = barray[offset]
		offset += 1
		self.fav_weather = barray[offset]
		offset += 1
		self.fav_time = barray[offset]
		offset += 1
		self.fav_temp = barray[offset]
		offset += 1

		self.fav_food = barray[offset]
		offset += 1
		self.fav_drink = barray[offset]
		offset += 1

		for i in range(0, len(self.weights)):
			self.weights[i] = barray[offset]
			offset += 1

	def __str__(self):

		return f"""pex_id: {self.pex_id} sprite: {self.sprite}
fav_scene: {self.fav_scene} fav_weather: {self.fav_weather}
fav_time: {self.fav_time} fav_temp: {self.fav_temp}
fav_food: {self.fav_food} fav_drink: {self.fav_drink}
weights: {self.weights}"""

class PetPEXStatePkt:
	def __init__(self):

		self.scene = 0
		self.scene_weather = 0
		self.scene_mood = 0
		self.scene_time = 0
		self.scene_temp = 0

		self.held_food = 0
		self.held_drink = 0

	def serialize(self):

		tx_bytes = bytearray()
		tx_bytes.append(PET_PKT_ID.PEX_STATE)

		tx_bytes.append(self.scene)
		tx_bytes.append(self.scene_weather)
		tx_bytes.append(self.scene_mood)
		tx_bytes.append(self.scene_time)

		tx_bytes.append(self.held_food)
		tx_bytes.append(self.held_drink)

		return tx_bytes

	def deserialize(self, rx_bytes):

		barray = rx_bytes

		if not isinstance(barray, bytearray):
			barray = bytearray(rx_bytes)

		offset = 1

		self.scene = barray[offset]
		offset += 1
		self.scene_weather = barray[offset]
		offset += 1
		self.scene_mood = barray[offset]
		offset += 1
		self.scene_time = barray[offset]
		offset += 1
		self.scene_temp = barray[offset]
		offset += 1

		self.held_food = barray[offset]
		offset += 1
		self.held_drink = barray[offset]
		offset += 1

	def __str__(self):

		return f"""scene: {self.scene} weather: {self.scene_weather}
mood: {self.scene_mood} time: {self.scene_time}
temp: {self.scene_temp} held_food: {self.held_food}
held_drink: {self.held_drink}"""

class PetPEXJournalEvtPkt:
	def __init__(self, entry=PetJournalEntry()):

		self.pet_id = 0
		self.index = 0
		self.entry = entry

	def serialize(self):

		tx_bytes = bytearray()
		tx_bytes.append(PET_PKT_ID.PEX_JOURNAL_EVT)

		e_u16(self.pet_id, tx_bytes)
		tx_bytes.append(self.index)

		e_u16(self.entry.timestamp, tx_bytes)
		tx_bytes.append(self.entry.event)

		return tx_bytes

	def deserialize(self, rx_bytes):

		barray = rx_bytes

		if not isinstance(barray, bytearray):
			barray = bytearray(rx_bytes)

		offset = 1

		self.pet_id = d_u16(barray, offset)
		offset += 2

		self.index = barray[offset]
		offset += 1

		self.entry.timestamp = d_u16(barray, offset)
		offset += 2
		self.entry.event = barray[offset]
		offset += 1

	def __str__(self):
		return f"index: {self.index} timestamp: {self.entry.timestamp} event: {self.entry.event}"

BLE_PKT_PPY_PERSONALITY_RX = PetPPYPersonalityPkt()

BLE_PKT_PEX_STATE_RX = PetPEXStatePkt()
BLE_PKT_PEX_JOURNAL_EVT_RX = PetPEXJournalEvtPkt()

BLE_PKT_WFC_RTC_RX = PetWFCRtcPkt()

async def find_ble_pet(pex_id):

	print(f"Trying to find pet {pex_id}...")
	sienna_devices = []

	if pex_id == SIENNA_MASTER_PEX:
		# we are in discover mode
		timeout = 3
	else:
		timeout = 1

	devices = await BleakScanner.discover(timeout=timeout, return_adv=True)

	for device in devices:

		pkt_data = devices[device]

		adv_data = pkt_data[1]
		mf_data = adv_data.manufacturer_data

		try:
			pet_md = mf_data[BLE_SIENNA_MF_ID]
			print(pet_md)
		except KeyError:
			continue

		adv_pex_id = (pet_md[PET_BLE_ADV_POS.PEX_ID_HIGH] << 8) | pet_md[PET_BLE_ADV_POS.PEX_ID_LOW]

		print(f"Found pet device {adv_pex_id} {PET_BLE_ADV_POS.PEX_ID_HIGH}")

		if pex_id == SIENNA_MASTER_PEX:
			sienna_devices.append(adv_pex_id)

		if adv_pex_id != pex_id:
			continue

		sienna_devices.append(device)

	if pex_id == SIENNA_MASTER_PEX:
		return sienna_devices

	if len(sienna_devices) == 0:
		return None

	return sienna_devices[0]

async def pet_ble_retrieve_journal(client):

	rtc_request_pkt = PetWFCDemoCmdPkt()
	rtc_request_pkt.cmd_id = PET_WFC_DEMO_CMDS.GET_TIME

	await client.write_gatt_char(BLE_UUID_CHR_WFC_TX, rtc_request_pkt.serialize(), response=False)
	await GAME_EVT_WFC_RX_RTC.wait()

	jrnl_request_pkt = PetPEXJournalEvtPkt()
	jrnl_request_pkt.index = JOURNAL_REQUEST_MAGIC_NUM

	tx_bytes = jrnl_request_pkt.serialize()

	await client.write_gatt_char(BLE_UUID_CHR_PEX_TX, tx_bytes, response=False)
	await GAME_EVT_PEX_RX_JOURNAL.wait()

	return copy.deepcopy(GAME_RX_JOURNAL)

def pet_ble_pex_notify_cb(characteristic, data):

	print(f"PEX: Got notify to {characteristic} with {data}")

	pkt_type = data[0]

	if pkt_type == PET_PKT_ID.PEX_STATE:

		BLE_PKT_PEX_STATE_RX.deserialize(data)
		GAME_EVT_PEX_RX_STATE.set()

	elif pkt_type == PET_PKT_ID.PEX_JOURNAL_EVT:

		journal_evt_pkt = PetPEXJournalEvtPkt()
		journal_evt_pkt.deserialize(data)

		try:
			GAME_RX_JOURNAL[journal_evt_pkt.pet_id]
		except KeyError:
			GAME_RX_JOURNAL[journal_evt_pkt.pet_id] = PetJournal()
			GAME_RX_JOURNAL[journal_evt_pkt.pet_id].epoch = BLE_PKT_WFC_RTC_RX.to_date()

		GAME_RX_JOURNAL[journal_evt_pkt.pet_id].add(journal_evt_pkt.entry)

		if journal_evt_pkt.index == JOURNAL_FINISHED_MAGIC_NUM:
			GAME_EVT_PEX_RX_JOURNAL.set()

def pet_ble_ppy_notify_cb(characteristic, data):

	print(f"PPY: Got notify to {characteristic} with {data}")

	pkt_type = data[0]

	if pkt_type == PET_PKT_ID.PPY_PERSONALITY:
		BLE_PKT_PPY_PERSONALITY_RX.deserialize(data)
		GAME_EVT_PPY_RX_PERSONALITY.set()

def pet_ble_wfc_notify_cb(characteristic, data):

	print(f"WFC: Got notify to {characteristic} with {data}")

	pkt_type = data[0]

	if pkt_type == PET_PKT_ID.WFC_RTC_UPDATE:
		BLE_PKT_WFC_RTC_RX.deserialize(data)
		GAME_EVT_WFC_RX_RTC.set()

async def pet_retrieve_rtc(client):

	rtc_request_pkt = PetWFCDemoCmdPkt()
	rtc_request_pkt.cmd_id = PET_WFC_DEMO_CMDS.GET_TIME

	await client.write_gatt_char(BLE_UUID_CHR_WFC_TX, rtc_request_pkt.serialize(), response=False)
	await GAME_EVT_WFC_RX_RTC.wait()

	return BLE_PKT_WFC_RTC_RX.to_date()

async def pet_retrieve_command(pex_id, command):

	pet = await find_ble_pet(pex_id)

	if pet is None:
		print("Unable to find pet")
		return

	print("Found pet!")

	async with BleakClient(pet) as client:

		await client.start_notify(BLE_UUID_CHR_PPY_RX, pet_ble_ppy_notify_cb)
		await client.start_notify(BLE_UUID_CHR_PEX_RX, pet_ble_pex_notify_cb)
		await client.start_notify(BLE_UUID_CHR_WFC_RX, pet_ble_wfc_notify_cb)

		return await command(client)

async def pet_send_packet(pex_id, packet, uuid):
	
	start_time = time.time()
	while True:
		pet = await find_ble_pet(pex_id)

		if pet is None:
			if (time.time() - start_time) > 10:
				return
			continue

		print("Found pet!")
		break

	async with BleakClient(pet) as client:

		await client.start_notify(BLE_UUID_CHR_PPY_RX, pet_ble_ppy_notify_cb)
		await client.start_notify(BLE_UUID_CHR_PEX_RX, pet_ble_pex_notify_cb)
		await client.start_notify(BLE_UUID_CHR_WFC_RX, pet_ble_wfc_notify_cb)

		await client.write_gatt_char(uuid, packet.serialize(), response=False)



async def pet_ble_set_personality(pex_id, ppy_pkt):
	await pet_ble_init()
	await pet_send_packet(pex_id, ppy_pkt, BLE_UUID_CHR_PPY_TX)

async def pet_ble_set_state(pex_id, state_pkt: PetPEXStatePkt):
	await pet_ble_init()

	demo_pkt = PetWFCDemoCmdPkt()

	demo_pkt.cmd_id = PET_WFC_DEMO_CMDS.CHANGE_SCENE
	demo_pkt.cmd_arg = state_pkt.scene
	await pet_send_packet(pex_id, demo_pkt, BLE_UUID_CHR_WFC_TX)

	demo_pkt.cmd_id = PET_WFC_DEMO_CMDS.CHANGE_MOOD
	demo_pkt.cmd_arg = state_pkt.scene_mood
	await pet_send_packet(pex_id, demo_pkt, BLE_UUID_CHR_WFC_TX)

	demo_pkt.cmd_id = PET_WFC_DEMO_CMDS.CHANGE_TIME
	demo_pkt.cmd_arg = state_pkt.scene_time
	await pet_send_packet(pex_id, demo_pkt, BLE_UUID_CHR_WFC_TX)

	# await pet_send_packet(pex_id, state_pkt, BLE_UUID_CHR_PEX_TX)

async def pet_ble_update_relationship(pex_id, relation_pkts):
	await pet_ble_init()
	for pkt in relation_pkts:
		await pet_send_packet(pex_id, pkt, BLE_UUID_CHR_WFC_TX)

async def pet_ble_discover_pets():
	await pet_ble_init()
	return await find_ble_pet(SIENNA_MASTER_PEX)

async def main():

	await pet_ble_init()

	# demo_pkt = PetWFCDemoCmdPkt()
	# demo_pkt.cmd_id = PET_WFC_DEMO_CMDS.CHANGE_MOOD
	# demo_pkt.cmd_arg = 4
	# await pet_send_packet(0xBABA, demo_pkt, BLE_UUID_CHR_WFC_TX)
	# demo_pkt.cmd_arg = 0
	# await pet_send_packet(0xBABA, demo_pkt, BLE_UUID_CHR_WFC_TX)

	# ppy_pkt = PetPPYPersonalityPkt()
	# ppy_pkt.randomize()

	# ppy_pkt.sprite = 2
	# await pet_ble_set_personality(0xBABA, ppy_pkt)

	pet_journal = await pet_retrieve_command(0xBABA, pet_ble_retrieve_journal)

	for pet in pet_journal:
		print(f"JOURNAL OF PET {hex(pet)}")
		print(pet_journal[pet])

	# pets_in_area = await pet_ble_discover_pets()
	# print(pets_in_area)
#
	#await pet_ble_set_personality(0xBABA, ppy_pkt)

	#time = await pet_retrieve_command(0xBABA, pet_retrieve_rtc)
	#print(time)

if __name__ == "__main__":
	asyncio.run(main())
