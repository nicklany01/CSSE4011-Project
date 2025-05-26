#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>

#include "comms.h"

#include "os_ble.h"
#include "os_uart.h"

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

void print_buffer(uint8_t *buffer, int len) {
	int i;
	for (i = 0; i < len - 1; i++) {
		printf("0x%x-", buffer[i]);
	}
	printf("0x%x\r\n", buffer[i]);
}

void process_uart_packet() {

	print_buffer(uart_passthru_rx.buff, uart_passthru_rx.len);

	switch (uart_passthru_rx.buff[0]) {

		case PET_PKT_PPY_PERSONALITY:

			printf("Got a PPY\r\n");

			deserialize_pet_personality_pkt(&pet_ppy_pkt, uart_passthru_rx.buff);

			my_mf_data[BLE_ADV_PEX_ID_HIGH] = pet_ppy_pkt.id >> 8;
			my_mf_data[BLE_ADV_PEX_ID_LOW] = pet_ppy_pkt.id & 0xFF;
			my_mf_data[BLE_ADV_MY_SPRITE] = pet_ppy_pkt.sprite;
			break;

		case PET_PKT_PEX_STATE:

			printf("Got a PEX.\r\n");

			deserialize_pet_exchange_state_pkt(&pet_pex_state_pkt, uart_passthru_rx.buff);

			my_mf_data[BLE_ADV_CURR_SCENE] = pet_pex_state_pkt.scene;
			my_mf_data[BLE_ADV_CURR_TIME] = pet_pex_state_pkt.scene_time;
			my_mf_data[BLE_ADV_CURR_FOOD] = pet_pex_state_pkt.held_food;
			my_mf_data[BLE_ADV_CURR_DRINK] = pet_pex_state_pkt.held_drink;
			break;

		default:
			break;
	}

	os_ble_update_mf_data(my_mf_data);
	os_ble_restart_advertising();
}

int main() {

	os_ble_passthru_s ble_rx;

	os_ble_init();
	if (!os_uart_init()) {
		printf("UART INIT FAILED!\n");
	}

	uint8_t tx_buff[RX_BUFF_SIZE] = "hello!";

	while (1) {

		if (k_msgq_num_used_get(&os_ble_rxq) > 0) {
			k_msgq_get(&os_ble_rxq, &ble_rx, K_NO_WAIT);

			uart_passthru_tx.len = ble_rx.len;
			memcpy(uart_passthru_tx.buff, ble_rx.buff, uart_passthru_tx.len);
			os_uart_passthru(&uart_passthru_tx);

			printf("Got a ble msg.\r\n");
		}

		if (k_msgq_num_used_get(&os_uart_rxq) > 0) {
			k_msgq_get(&os_uart_rxq, &uart_passthru_rx, K_NO_WAIT);

			printf("Got a uart msg.\r\n");

			process_uart_packet();
		}

		k_sleep(K_MSEC(100));

	}
}
