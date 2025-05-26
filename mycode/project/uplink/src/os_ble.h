#include <stdint.h>

#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#define MF_ID_HIGH 0x43
#define MF_ID_LOW 0x69

#define MF_DLEN 12

#define RX_BUFF_SIZE 256
#define RX_QUEUE_SIZE 5

typedef struct {

	uint8_t buff[256];
	int len;
} os_ble_passthru_s;

extern struct k_msgq os_ble_rxq;



bool os_ble_init();
