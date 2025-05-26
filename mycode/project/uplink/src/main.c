#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>

#include "os_ble.h"
#include "os_uart.h"

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

			os_uart_tx((uint8_t *)&ble_rx.len, 1);
			os_uart_tx(ble_rx.buff, ble_rx.len);
		}

		printf("Sending uart...\n");

		k_sleep(K_MSEC(500));

	}
}
