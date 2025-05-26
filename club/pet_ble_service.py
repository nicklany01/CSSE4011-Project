import asyncio
from bleak import BleakScanner, BleakClient

from random import randint

BLE_SIENNA_MF_ID = 0x6943
SIENNA_MASTER_PEX = 0x4369

BLE_UUID_SRV_PPY = "4A259CE4-4369-4153-AD28-B8D61B4F447A"
BLE_UUID_CHR_PPY_RX = "4A259CE4-4770-4153-AD28-B8D61B4F447A"
BLE_UUID_CHR_PPY_TX = "4A259CE4-4771-4153-AD28-B8D61B4F447A"

# WARNING: keep sync'd with comms.h
class PET_WFC_DEMO_CMDS:
	CHANGE_SCENE = 0
	CHANGE_MOOD = 1
	CHANGE_TIME = 2

class SPRITE:
	ZERO = 0
	CHERRY = 1
	ICE = 2
	GRAPE = 3
	BAJA_BLAST = 4


class PET_PKT_ID:
	PPY_PERSONALITY = 0
	PEX_STATE = 1
	PEX_JOURNAL = 2
	PEX_JOURNAL_EVT = 3
	WFC_DEMO_COMMAND = 4

class PET_BLE_ADV_POS:
	PEX_ID_HIGH = 0
	PEX_ID_LOW = 1
	MY_SPRITE = 2

	CURR_SCENE = 3
	CURR_TIME = 4

	CURR_FOOD = 5
	CURR_DRINK = 6

def e_u16(value, buff):
	buff.extend(value.to_bytes(2, "big"))

class PetWFCDemoCmdPkt:

	def __init__(self):
		self.cmd_id = PET_WFC_DEMO_CMDS.CHANGE_SCENE
		self.cmd_arg = 0

	def serialize(self):

		ret_bytes = bytearray()

		ret_bytes.append(PET_PKT_ID.WFC_DEMO_COMMAND)
		ret_bytes.append(self.cmd_id)
		e_u16(self.cmd_arg, ret_bytes)

		return ret_bytes

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

		ret_bytes = bytearray()

		ret_bytes.append(PET_PKT_ID.PPY_PERSONALITY)

		e_u16(self.pex_id, ret_bytes)

		ret_bytes.append(self.sprite)
		ret_bytes.append(self.fav_scene)
		ret_bytes.append(self.fav_weather)
		ret_bytes.append(self.fav_time)
		ret_bytes.append(self.fav_temp)
		ret_bytes.append(self.fav_drink)

		for i in self.weights:
			ret_bytes.append(i)

		return ret_bytes

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

async def main():

	demo_pkt = PetWFCDemoCmdPkt()
	ppy_pkt = PetPPYPersonalityPkt()

	ppy_pkt.randomize()

	ppy_pkt.fav_drink = 3
	ppy_pkt.fav_food = 2
	ppy_pkt.sprite = SPRITE.BAJA_BLAST

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
