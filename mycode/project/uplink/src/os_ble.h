#include <stdint.h>
#include <stdbool.h>

#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#define MF_ID_HIGH 0x43
#define MF_ID_LOW 0x69

#define SIENNA_MF_ID 0x4369
#define SIENNA_MF_ID_LIL_ENDY 0x6943

#define MF_DLEN 12

#define RX_BUFF_SIZE 256
#define RX_QUEUE_SIZE 5

#define OS_BLE_DURATION_SCAN 1000
#define OS_BLE_DURATION_ADVERTISE 1000

typedef enum {

	OS_BLE_STATE_UNINIT,
	OS_BLE_STATE_SCAN,
	OS_BLE_STATE_ADVERTISE,
	OS_BLE_STATE_CONNECTED,
	OS_BLE_STATE_TARGETING
} os_ble_state_e;

typedef struct {

	uint8_t buff[256];
	int len;
} os_ble_passthru_s;

typedef struct {

	int8_t rssi;
	uint8_t mf_data[MF_DLEN];
} os_ble_pet_adv_s;

typedef struct {

	os_ble_state_e state;
} os_ble_state_s;

extern os_ble_state_s os_ble_state;
extern struct k_msgq os_ble_rxq;
extern struct k_msgq os_ble_advq;

bool os_ble_init();
void os_ble_update_mf_data(uint8_t *new_mf_data);

void os_ble_start_scan();
void os_ble_stop_scan();

int os_ble_restart_advertising();
void os_ble_stop_advertising();
