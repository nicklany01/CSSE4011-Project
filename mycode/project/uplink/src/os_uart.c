#include <stddef.h>
#include <stdint.h>

#include "os_uart.h"

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

struct k_msgq os_uart_rxq;

uint8_t os_uart_rx_buff[RX_BUFF_SIZE] = {0};
int rx_buff_idx = 0;

char os_uart_rxq_buff[RX_QUEUE_SIZE * sizeof(uint8_t) * RX_BUFF_SIZE];

os_uart_inf m_uart_state = {

	.partner_dev = OS_UART_PARTNER,
	.target_size = 0,
	.rx_error = false,
	.rx_overflow = false
};

void os_uart_tx(uint8_t *buff, int len) {
	for (int i = 0; i < len; i++) {
		uart_poll_out(m_uart_state.partner_dev, buff[i]);
	}
}

static void uart_cb(const struct device *dev, void *ctx) {

	uint8_t c;

	if (!uart_irq_update(m_uart_state.partner_dev)) {
		return;
	}

	if (!uart_irq_rx_ready(m_uart_state.partner_dev)) {
		return;
	}

	while (uart_fifo_read(m_uart_state.partner_dev, &c, 1) == 1) {
		os_uart_rx_buff[rx_buff_idx++] = c;

		if (rx_buff_idx == m_uart_state.target_size) {
			// done should put on a queue
			k_msgq_put(&os_uart_rxq, &os_uart_rx_buff, K_NO_WAIT);
			rx_buff_idx = 0;
			break;
		}
	}
};

bool os_uart_init() {

	if (!device_is_ready(m_uart_state.partner_dev)) {
		return false;
	}

	k_msgq_init(&os_uart_rxq, os_uart_rxq_buff, sizeof(uint8_t) * RX_BUFF_SIZE, RX_QUEUE_SIZE);

	if (uart_irq_callback_user_data_set(m_uart_state.partner_dev, uart_cb, NULL) < 0) {
		return false;
	}

	uart_irq_rx_enable(m_uart_state.partner_dev);

	return true;
}
