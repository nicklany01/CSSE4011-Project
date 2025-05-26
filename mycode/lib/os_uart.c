#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "os_uart.h"

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

struct k_msgq os_uart_rxq;

uint8_t os_uart_rx_buff[RX_BUFF_SIZE] = {0};
int rx_buff_idx = 0;

char os_uart_rxq_buff[RX_QUEUE_SIZE * sizeof(os_uart_passthru_s)];

os_uart_inf m_uart_state = {

	.partner_dev = OS_UART_PARTNER,
	.target_size = -1,
	.rx_error = false,
	.rx_overflow = false
};

uint8_t os_uart_evt_flags = 0;

void os_uart_tx(uint8_t *buff, int len) {

	for (int i = 0; i < len; i++) {
		uart_poll_out(m_uart_state.partner_dev, buff[i]);
	}
}

void os_uart_passthru(os_uart_passthru_s *ps) {
	os_uart_tx((uint8_t *)&ps->len, 1);
	os_uart_tx(ps->buff, ps->len);
}

static void uart_cb(const struct device *dev, void *ctx) {

	uint8_t c;

	os_uart_passthru_s passthru_rx = {
		.len = -1,
		.buff = {0}
	};

	if (!uart_irq_update(m_uart_state.partner_dev)) {
		return;
	}

	if (!uart_irq_rx_ready(m_uart_state.partner_dev)) {
		return;
	}

	while (uart_fifo_read(m_uart_state.partner_dev, &c, 1) == 1) {
		if (m_uart_state.target_size == -1) {
			m_uart_state.target_size = c;
			break;
		}

		os_uart_rx_buff[rx_buff_idx++] = c;

		if (rx_buff_idx == m_uart_state.target_size) {
			// done should put on a queue
			memcpy(passthru_rx.buff, os_uart_rx_buff, m_uart_state.target_size);
			passthru_rx.len = m_uart_state.target_size;

			rx_buff_idx = 0;
			m_uart_state.target_size = -1;

			k_msgq_put(&os_uart_rxq, &passthru_rx, K_NO_WAIT);
			break;
		}
	}
};

bool os_uart_init() {

	if (!device_is_ready(m_uart_state.partner_dev)) {
		return false;
	}

	k_msgq_init(&os_uart_rxq, os_uart_rxq_buff, sizeof(os_uart_passthru_s), RX_QUEUE_SIZE);

	if (uart_irq_callback_user_data_set(m_uart_state.partner_dev, uart_cb, NULL) < 0) {
		return false;
	}

	uart_irq_rx_enable(m_uart_state.partner_dev);

	return true;
}
