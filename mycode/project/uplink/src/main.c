#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>

#include "comms.h"

#include "friends.h"
#include "journal.h"
#include "os_ble.h"
#include "os_uart.h"
#include "ble_uuid.h"
#include "pet_wfc.h"

os_uart_passthru_s uart_passthru_rx = {

	.len = -1,
	.buff = {0}
};

os_uart_passthru_s uart_passthru_tx = {

	.len = -1,
	.buff = {0}
};

uint8_t my_mf_data[MF_DLEN] = {

	MF_ID_HIGH,
	MF_ID_LOW,
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
};

pet_personality_pkt_s pet_ppy_pkt;
pet_exchange_state_pkt_s pet_pex_state_pkt;
pet_exchange_journal_evt_pkt_s pet_pex_journal_evt_pkt;
pet_wfc_pkt_demo_cmd_s pet_wfc_demo_pkt;

pet_uart_srvc_rssi_pkt_s pet_uart_srvc_rssi_pkt;

pet_wfc_state_e pet_wfc_state = PET_WFC_NULL;

void print_buffer(uint8_t *buffer, int len) {
	int i;
	for (i = 0; i < len - 1; i++) {
		printf("0x%x-", buffer[i]);
	}
	printf("0x%x\r\n", buffer[i]);
}

void process_uart_packet() {

	os_ble_passthru_s ble_tx;

	print_buffer(uart_passthru_rx.buff, uart_passthru_rx.len);

	switch (uart_passthru_rx.buff[0]) {

		case PET_PKT_WFC_DEMO_COMMAND:
			printf("Got a WFC cmd \r\n");
			deserialize_pet_wfc_demo_cmd_pkt(&pet_wfc_demo_pkt, uart_passthru_rx.buff);

			switch (pet_wfc_demo_pkt.cmd_id) {
				case PET_WFC_CMD_INIT_CONN:
					// want to initiate pet wireless comunications

					targeting = pet_wfc_demo_pkt.cmd_arg;
					os_ble_state.state = OS_BLE_STATE_TARGETING;
					break;
				default:
					break;
			}

			break;

		case PET_PKT_WFC_RTC_UPDATE:
			printf("Got a WFC\r\n");

			ble_tx.charac = BLE_UUID_16_CHR_WFC_RX;
			ble_tx.len = uart_passthru_rx.len;
			memcpy(ble_tx.buff, uart_passthru_rx.buff, ble_tx.len);

			os_ble_notify(&ble_tx);
			break;

		case PET_PKT_PPY_PERSONALITY:

			printf("Got a PPY\r\n");

			deserialize_pet_personality_pkt(&pet_ppy_pkt, uart_passthru_rx.buff);

			my_mf_data[BLE_ADV_PEX_ID_HIGH] = pet_ppy_pkt.id >> 8;
			my_mf_data[BLE_ADV_PEX_ID_LOW] = pet_ppy_pkt.id & 0xFF;
			my_mf_data[BLE_ADV_MY_SPRITE] = pet_ppy_pkt.sprite;

			if (os_ble_state.state == OS_BLE_STATE_CONNECTED) {
				ble_tx.charac = BLE_UUID_16_CHR_PPY_RX;
				ble_tx.len = uart_passthru_rx.len;
				memcpy(ble_tx.buff, uart_passthru_rx.buff, ble_tx.len);

				os_ble_notify(&ble_tx);
			}
			break;

		case PET_PKT_PEX_STATE:

			printf("Got a PEX.\r\n");

			//deserialize_pet_exchange_state_pkt(&pet_pex_state_pkt, uart_passthru_rx.buff);
			memcpy(my_mf_data + BLE_ADV_PEX_STATE_START, uart_passthru_rx.buff + 1, 7);

			if (os_ble_state.state == OS_BLE_STATE_CONNECTED) {
				ble_tx.charac = BLE_UUID_16_CHR_PEX_RX;
				ble_tx.len = uart_passthru_rx.len;
				memcpy(ble_tx.buff, uart_passthru_rx.buff, ble_tx.len);

				os_ble_notify(&ble_tx);
			}
			break;

		case PET_PKT_PEX_JOURNAL_EVT:
			printf("Got a JOURNAL EVT.\r\n");

			ble_tx.charac = BLE_UUID_16_CHR_PEX_RX;
			ble_tx.len = uart_passthru_rx.len;
			memcpy(ble_tx.buff, uart_passthru_rx.buff, ble_tx.len);

			os_ble_notify(&ble_tx);
			break;

		default:
			break;
	}

	os_ble_update_mf_data(my_mf_data);
}


K_THREAD_STACK_DEFINE(stack_bluetooth_state_handler, 2048);
struct k_thread thread_bluetooth_state_handler_data;

static void thread_bluetooth_state_handler(void *a, void *b, void *c) {

	os_ble_passthru_s ble_rx;
	os_ble_passthru_s ble_tx;
	os_ble_pet_adv_s pet_adv;

	while (true) {

		switch (os_ble_state.state) {

			case OS_BLE_STATE_SCAN:

				printf("Starting scan...\r\n");
				os_ble_stop_advertising();
				os_ble_start_scan();

				if (!k_msgq_get(&os_ble_advq, &pet_adv, K_MSEC(OS_BLE_DURATION_SCAN))) {

					printf("Got sienna packet.\r\n");

					pet_uart_srvc_rssi_pkt.rssi = pet_adv.rssi;
					pet_uart_srvc_rssi_pkt.id = d_u16(pet_adv.mf_data + 2);
					pet_uart_srvc_rssi_pkt.sprite = pet_adv.mf_data[BLE_ADV_MY_SPRITE];

					deserialize_pet_exchange_state_pkt(&pet_uart_srvc_rssi_pkt.pex_state,
						pet_adv.mf_data + (BLE_ADV_PEX_STATE_START - 1));

					uart_passthru_tx.len = serialize_pet_uart_srvc_rssi_pkt(&pet_uart_srvc_rssi_pkt, uart_passthru_tx.buff);
					os_uart_passthru(&uart_passthru_tx);
				}

				os_ble_state.state = os_ble_state.state == OS_BLE_STATE_SCAN
					? OS_BLE_STATE_ADVERTISE
					: os_ble_state.state;

				break;

			case OS_BLE_STATE_ADVERTISE:

				printf("Starting advertise...\r\n");
				os_ble_stop_scan();
				os_ble_restart_advertising();

				k_sleep(K_MSEC(OS_BLE_DURATION_ADVERTISE));

				os_ble_state.state = os_ble_state.state == OS_BLE_STATE_ADVERTISE
					? OS_BLE_STATE_SCAN
					: os_ble_state.state;

				break;

			case OS_BLE_STATE_TARGETING:

				os_ble_stop_advertising();
				// but now we are looking for a connection
				os_ble_start_scan();

				k_sleep(K_MSEC(1000));

				break;

			case OS_BLE_STATE_PET_WFC:

				switch (pet_wfc_state) {

					case PET_WFC_NOTIFY_M5:

						pet_wfc_demo_pkt.cmd_id = PET_WFC_CMD_INIT_CONN;
						pet_wfc_demo_pkt.cmd_arg = targeting;

						uart_passthru_tx.len = serialize_pet_wfc_demo_cmd_pkt(
							&pet_wfc_demo_pkt, uart_passthru_tx.buff);

						os_uart_passthru(&uart_passthru_tx);
						pet_wfc_state = PET_WFC_SEND_HELLO;
						break;

					case PET_WFC_SEND_HELLO:
						// we need to tell the other device
						// that we are a pet

						pet_wfc_demo_pkt.cmd_id = PET_WFC_CMD_HELLO;

						ble_tx.charac = BLE_UUID_16_CHR_PEX_RX;
						ble_tx.len = serialize_pet_wfc_demo_cmd_pkt(&pet_wfc_demo_pkt, ble_tx.buff);

						os_ble_write(&ble_tx);
						pet_wfc_state = PET_WFC_WAIT_HELLO;
						break;

					case PET_WFC_WAIT_HELLO:

						if (!k_msgq_get(&os_ble_rxq, &ble_rx, K_MSEC(100))) {

							if (ble_rx.buff[1] == PET_PKT_WFC_DEMO_COMMAND && ble_rx.buff[2] == PET_WFC_CMD_HELLO) {
								// other pet has acked lets do journal exchange
								pet_wfc_state = PET_WFC_WAIT_JOURNAL;
								// we can give a journal request to the M5 and it will tx straight thru
								pet_pex_journal_evt_pkt.index = JOURNAL_REQUEST_MAGIC_NUM;
								pet_pex_journal_evt_pkt.entry.event = JOURNAL_EVT_HOT; // we're pretty hot ngl
								pet_pex_journal_evt_pkt.entry.timestamp = 0;
								pet_pex_journal_evt_pkt.id = 0;

								uart_passthru_tx.len = serialize_pet_exchange_journal_evt_pkt(&pet_pex_journal_evt_pkt, uart_passthru_tx.buff);
								os_uart_passthru(&uart_passthru_tx);
							}
						}

						break;

					case PET_WFC_WAIT_JOURNAL:

						if (!k_msgq_get(&os_ble_rxq, &ble_rx, K_MSEC(100))) {
							// we got a journal event so forward straight to the M5
							uart_passthru_tx.len = ble_rx.len;
							memcpy(uart_passthru_tx.buff, ble_rx.buff, uart_passthru_rx.len);
							os_uart_passthru(&uart_passthru_tx);

							deserialize_pet_exchange_journal_evt_pkt(&pet_pex_journal_evt_pkt, ble_rx.buff);

							if (pet_pex_journal_evt_pkt.index == JOURNAL_FINISHED_MAGIC_NUM) {
								// we're done here :)
								pet_wfc_state = PET_WFC_NULL;
								bt_conn_disconnect(pet_wfc_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
							}
						}

						break;

					case PET_WFC_GOODBYE_M5:

						pet_wfc_demo_pkt.cmd_id = PET_WFC_CMD_CLOSE_CONN;
						pet_wfc_demo_pkt.cmd_arg = 0;
						targeting = 0;
						printf("Goodbye M5!\r\n");

						uart_passthru_tx.len = serialize_pet_wfc_demo_cmd_pkt(
							&pet_wfc_demo_pkt, uart_passthru_tx.buff);

						os_uart_passthru(&uart_passthru_tx);

						pet_wfc_state = PET_WFC_NULL;
						os_ble_state.state = OS_BLE_STATE_SCAN;
						break;

					default:
						break;
				}

				k_sleep(K_MSEC(100));
				break;

			case OS_BLE_STATE_CONNECTED:

				if (k_msgq_num_used_get(&os_ble_rxq) > 0) {
					k_msgq_get(&os_ble_rxq, &ble_rx, K_NO_WAIT);

					if (ble_rx.buff[1] == PET_PKT_WFC_DEMO_COMMAND && ble_rx.buff[2] == PET_WFC_CMD_HELLO) {
						// we're actually in Pet WFC mode
						pet_wfc_state = PET_WFC_SEND_HELLO;
						os_ble_state.state = OS_BLE_STATE_PET_WFC;
						break;
					}

					uart_passthru_tx.len = ble_rx.len;
					memcpy(uart_passthru_tx.buff, ble_rx.buff, uart_passthru_tx.len);
					os_uart_passthru(&uart_passthru_tx);

					printf("Got a ble msg.\r\n");
					print_buffer(uart_passthru_tx.buff, uart_passthru_tx.len);
				}
			// fallthrough
			default:
				k_sleep(K_MSEC(100));
				break;
		}
	}
}

int main() {

	k_tid_t tid_bluetooth_state_handler;

	os_ble_init();

	if (!os_uart_init()) {
		printf("UART INIT FAILED!\n");
	}

	os_ble_state.state = OS_BLE_STATE_SCAN;

	tid_bluetooth_state_handler = k_thread_create(
		&thread_bluetooth_state_handler_data,
		stack_bluetooth_state_handler,
		K_THREAD_STACK_SIZEOF(stack_bluetooth_state_handler),
		thread_bluetooth_state_handler,
		NULL, NULL, NULL,
		2, 0, K_NO_WAIT
	);

	while (1) {

		k_msgq_get(&os_uart_rxq, &uart_passthru_rx, K_FOREVER);
		printf("Got a uart msg.\r\n");

		process_uart_packet();
	}
}
