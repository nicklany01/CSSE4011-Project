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

bool os_ble_init();
