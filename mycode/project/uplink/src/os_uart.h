#include <stdbool.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>

#define OS_UART_PARTNER DEVICE_DT_GET(DT_ALIAS(u4))

#define RX_BUFF_SIZE 256
#define RX_QUEUE_SIZE 5

typedef struct {

	const struct device *partner_dev;
	int target_size;
	bool rx_error;
	bool rx_overflow;
} os_uart_inf;

extern struct k_msgq os_uart_rxq;
extern uint8_t os_uart_evt_flags;

bool os_uart_init();
void os_uart_tx(uint8_t *buff, int len);
