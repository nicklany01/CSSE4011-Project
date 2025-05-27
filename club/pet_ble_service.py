import asyncio

from random import randint
from datetime import datetime

from bleak import BleakScanner, BleakClient

from pet_app_helpers import PET_WFC_DEMO_CMDS, SPRITE, PET_PKT_ID, PET_BLE_ADV_POS, \
	PetJournalEntry, PetJournal

BLE_SIENNA_MF_ID = 0x6943
SIENNA_MASTER_PEX = 0x4369

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

		self.index = 0
		self.entry = entry

	def serialize(self):

		tx_bytes = bytearray()
		tx_bytes.append(PET_PKT_ID.PEX_JOURNAL_EVT)

		tx_bytes.append(self.index)

		e_u16(self.entry.timestamp, tx_bytes)
		tx_bytes.append(self.entry.event)

		return tx_bytes

	def deserialize(self, rx_bytes):

		barray = rx_bytes

		if not isinstance(barray, bytearray):
			barray = bytearray(rx_bytes)

		offset = 1

		self.index = barray[offset]
		offset += 1

		self.entry.timestamp = d_u16(barray, offset)
		offset += 2
		self.entry.event = barray[offset]
		offset += 1

	def __str__(self):
		return f"index: {self.index} timestamp: {self.entry.timestamp} event: {self.entry.event}"

async def find_ble_pet(pex_id):

	print(f"Trying to find pet {pex_id}...")

	connect_dev = None
	devices = await BleakScanner.discover(timeout=1, return_adv=True)

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

		if adv_pex_id != pex_id:
			continue

		connect_dev = device

	return connect_dev

def test_ppy_pkt():

	print("=== TESTING PPY PERSONALITY PKT ===")

	ppy_pkt = PetPPYPersonalityPkt()

	ppy_pkt.randomize()

	ppy_pkt.fav_drink = 3
	ppy_pkt.fav_food = 2
	ppy_pkt.sprite = SPRITE.BAJA_BLAST

	new_pkt = PetPPYPersonalityPkt()
	new_pkt.deserialize(ppy_pkt.serialize())

	print(str(ppy_pkt))
	print("")
	print(str(new_pkt))

	if str(ppy_pkt) == str(new_pkt):
		print("Packets are equal.")
		return True

	print("Packets are NOT equal!")
	return False

def notify_cb(characteristic, data):
	print(f"Got notify to {characteristic} with {data}")

async def main():

	demo_pkt = PetWFCDemoCmdPkt()

	test_ppy_pkt()

	ppy_pkt = PetPPYPersonalityPkt()
	ppy_pkt.randomize()

	jrnl_request_pkt = PetPEXJournalEvtPkt()
	jrnl_request_pkt.index = JOURNAL_REQUEST_MAGIC_NUM

	while True:

		pet = await find_ble_pet(0xBABE)

		if pet is None:
			continue

		break

	async with BleakClient(pet) as client:

		for service in client.services:
			print(service)
			for char in service.characteristics:
				print("\t", char)

		await client.start_notify(BLE_UUID_CHR_PPY_RX, notify_cb)
		await client.start_notify(BLE_UUID_CHR_PEX_RX, notify_cb)
		await client.start_notify(BLE_UUID_CHR_WFC_RX, notify_cb)

		tx_bytes = jrnl_request_pkt.serialize()
		print(tx_bytes)

		await client.write_gatt_char(BLE_UUID_CHR_PPY_TX, tx_bytes, response=False)

		await asyncio.sleep(100)
		return

		# SEND PPY UPDATE
		tx_bytes = ppy_pkt.serialize()
		print(tx_bytes)
		await client.write_gatt_char(BLE_UUID_CHR_PPY_TX, tx_bytes, response=False)

		for i in range(0, 5):

			if i == 3:
				continue

			demo_pkt.cmd_id = PET_WFC_DEMO_CMDS.CHANGE_SCENE
			demo_pkt.cmd_arg = i

			tx_bytes = demo_pkt.serialize()
			print(tx_bytes)

			await client.write_gatt_char(BLE_UUID_CHR_PPY_TX, tx_bytes, response=False)

			await asyncio.sleep(2)

if __name__ == "__main__":
	asyncio.run(main())
