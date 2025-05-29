import asyncio
import sys
import logging
import threading

import pet_app_helpers
import pet_ble_service

import serial

from queue import Queue

from bluez_peripheral.gatt.service import Service
from bluez_peripheral.gatt.characteristic import characteristic, CharacteristicFlags as CharFlags
from bluez_peripheral.advert import Advertisement
from bluez_peripheral.uuid import BTUUID

from bluez_peripheral.gatt.service import ServiceCollection

from bluez_peripheral.util import get_message_bus

class UplinkState:
	SCAN = 0
	ADVERTISE = 1
	CONNECTED = 2

UPLINK_STATE = UplinkState.SCAN

RSSI_QUEUE = Queue()
BLE_RX_QUEUE = Queue()
BLE_TX_QUEUE = Queue()

logger = logging.getLogger(__name__)
logging.basicConfig(stream=sys.stdout, level=logging.INFO)

PET_PORT = None

mf_data = [
	pet_ble_service.MF_ID_HIGH,
	pet_ble_service.MF_ID_LOW,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
]

class PEXService(Service):
	def __init__(self):
		super().__init__(pet_ble_service.BLE_UUID_SRV_PEX)

	@characteristic(pet_ble_service.BLE_UUID_CHR_PEX_RX, CharFlags.NOTIFY)
	def ble_srv_rx(self, options):
		pass

	@characteristic(pet_ble_service.BLE_UUID_CHR_PEX_TX, CharFlags.WRITE)
	def ble_srv_tx(self, options):
		pass

	@ble_srv_tx.setter
	def ble_srv_tx(self, value, options):
		PET_PORT.write(value)

class PPYService(Service):
	def __init__(self):
		super().__init__(pet_ble_service.BLE_UUID_SRV_PPY)

	@characteristic(pet_ble_service.BLE_UUID_CHR_PPY_RX, CharFlags.NOTIFY)
	def ble_srv_rx(self, options):
		pass

	@characteristic(pet_ble_service.BLE_UUID_CHR_PPY_TX, CharFlags.WRITE)
	def ble_srv_tx(self, options):
		pass

	@ble_srv_tx.setter
	def ble_srv_tx(self, value, options):
		PET_PORT.write(value)

class WFCService(Service):
	def __init__(self):
		super().__init__(pet_ble_service.BLE_UUID_SRV_WFC)

	@characteristic(pet_ble_service.BLE_UUID_CHR_WFC_RX, CharFlags.NOTIFY)
	def ble_srv_rx(self, options):
		pass

	@characteristic(pet_ble_service.BLE_UUID_CHR_WFC_TX, CharFlags.WRITE)
	def ble_srv_tx(self, options):
		pass

	@ble_srv_tx.setter
	def ble_srv_tx(self, value, options):
		logger.info(f"{value}")

		wfc_pkt = pet_ble_service.PetWFCDemoCmdPkt()
		wfc_pkt.deserialize(value)

		if wfc_pkt.cmd_id == pet_app_helpers.PET_WFC_DEMO_CMDS.CLOSE_CONN:
			sys.exit()

		#PET_PORT.write(value)

def list_to_bytes(input_list):
	byte_arr = bytearray()
	for integer in input_list:
		byte_arr.append(integer)

	return bytes(byte_arr)

def adv_set_pex_id(pex_id):

	mf_data[pet_ble_service.PET_BLE_ADV_POS.PEX_ID_HIGH] = pex_id >> 8
	mf_data[pet_ble_service.PET_BLE_ADV_POS.PEX_ID_LOW] = pex_id & 0xFF

def adv_set_sprite(sprite):

	mf_data[pet_ble_service.PET_BLE_ADV_POS.MY_SPRITE] = sprite & 0xFF

async def adv_start(bus, prev_adv: Advertisement):

	#if prev_adv is not None:
	#	#await prev_adv.unregister()

	advert = Advertisement(
		"sienna-upl",
		[],
		0,
		2,
		manufacturerData={pet_ble_service.BLE_SIENNA_MF_ID: list_to_bytes(mf_data)}
	)

	await advert.register(bus)

	return advert

def uart_handler_thread():

	while True:
		pkt_size = PET_PORT.read(1)
		if pkt_size == b'':
			continue

		rx_pkt = PET_PORT.read(pkt_size)

		if rx_pkt[0] == pet_app_helpers.PET_PKT_ID.WFC_RTC_UPDATE:
			logger.info("Got a WFC")
			# notify
			BLE_TX_QUEUE.put((pet_ble_service.BLE_UUID_CHR_WFC_RX, rx_pkt))

		elif rx_pkt[0] == pet_app_helpers.PET_PKT_ID.PPY_PERSONALITY:
			logger.info("Got a PPY")

			ppy_pkt = pet_ble_service.PetPPYPersonalityPkt()
			ppy_pkt.deserialize(rx_pkt)

			adv_set_pex_id(ppy_pkt.pex_id)
			adv_set_sprite(ppy_pkt.sprite)

			if UPLINK_STATE == UplinkState.CONNECTED:
				# notify
				BLE_TX_QUEUE.put((pet_ble_service.BLE_UUID_CHR_PPY_RX, rx_pkt))

		elif rx_pkt[0] == pet_app_helpers.PET_PKT_ID.PEX_STATE:
			logger.info("Got a PEX")

			if UPLINK_STATE == UplinkState.CONNECTED:
				# notify
				BLE_TX_QUEUE.put((pet_ble_service.BLE_UUID_CHR_PEX_RX, rx_pkt))

		elif rx_pkt[0] == pet_app_helpers.PET_PKT_ID.PEX_JOURNAL_EVT:
			logger.info("Got a JOURNAL EVT")

			BLE_TX_QUEUE.put((pet_ble_service.BLE_UUID_CHR_PEX_RX, rx_pkt))


async def main():

	global PET_PORT, UPLINK_STATE, RSSI_QUEUE

	#PET_PORT = serial.Serial(
	#	port=sys.argv[1],
	#	baudrate=115200
	#)

	pet_uart_srvc_rssi_pkt = pet_ble_service.PetUARTSrvcRssiPkt()

	pet_pex_service = PEXService()
	pet_ppy_service = PPYService()
	pet_wfc_service = WFCService()

	service_collection = ServiceCollection()
	service_collection.add_service(pet_pex_service)
	service_collection.add_service(pet_ppy_service)
	service_collection.add_service(pet_wfc_service)

	adv_set_pex_id(0xBABE)
	adv_set_sprite(pet_app_helpers.SPRITE.CHERRY)

	bus = await get_message_bus()
	advert = None

	await service_collection.register(bus)

	while True:

		if pet_ppy_service.ble_srv_rx._notify and pet_pex_service.ble_srv_rx._notify \
				and pet_wfc_service.ble_srv_rx._notify:
			UPLINK_STATE = UplinkState.CONNECTED
			logging.info("still on for notifs")
		elif UPLINK_STATE == UplinkState.CONNECTED:
			UPLINK_STATE = UplinkState.SCAN

		if UPLINK_STATE == UplinkState.SCAN:
			logger.info("Starting scan...")

			await asyncio.sleep(1)

			if not RSSI_QUEUE.empty():
				rssi_pkt = RSSI_QUEUE.get()

				pet_uart_srvc_rssi_pkt.rssi = rssi_pkt.rssi
				pet_uart_srvc_rssi_pkt.pex_id = rssi_pkt.pex_id

			if UPLINK_STATE == UplinkState.SCAN:
				UPLINK_STATE = UplinkState.ADVERTISE

		elif UPLINK_STATE == UplinkState.ADVERTISE:
			logger.info("Starting advertise...")

			advert = await adv_start(bus, advert)
			await asyncio.sleep(2)

			if UPLINK_STATE == UplinkState.ADVERTISE:
				UPLINK_STATE = UplinkState.SCAN

		elif UPLINK_STATE == UplinkState.CONNECTED:

			if not BLE_RX_QUEUE.empty():
				ble_data = BLE_RX_QUEUE.get()

				PET_PORT.write(len(ble_data))
				PET_PORT.write(ble_data)

			if not BLE_TX_QUEUE.empty():
				ble_tx_request = BLE_TX_QUEUE.get()

				if ble_tx_request[0] == pet_ble_service.BLE_UUID_CHR_PEX_RX:
					pet_pex_service.ble_srv_rx.changed(ble_tx_request[1])

				elif ble_tx_request[0] == pet_ble_service.BLE_UUID_CHR_PPY_RX:
					pet_pex_service.ble_srv_rx.changed(ble_tx_request[1])

				elif ble_tx_request[0] == pet_ble_service.BLE_UUID_CHR_WFC_RX:
					pet_pex_service.ble_srv_rx.changed(ble_tx_request[1])

		await asyncio.sleep(0.1)

if __name__ == "__main__":

	PET_PORT = serial.Serial(
		port=sys.argv[1],
		baudrate=115200,
		timeout=1
	)

	uart_handler = threading.Thread(target=uart_handler_thread, daemon=True)
	uart_handler.start()

	asyncio.run(main())
