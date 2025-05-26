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

#define BLE_LOCAL_NAME_UPLINK "sienna-upl"
#define BLE_LOCAL_NAME_BASE "sienna-bae"

#define OS_BLE_EVT_MSK_UPLINK (1 << 0)
#define OS_BLE_EVT_MSK_BASE (1 << 1)
#define OS_BLE_EVT_MSK_ADV_RX (1 << 2)

extern uint8_t os_ble_evt_flags;

typedef struct {


} os_ble_state_s;

bool os_ble_init();
