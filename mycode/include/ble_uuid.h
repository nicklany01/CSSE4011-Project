#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>


extern const struct bt_uuid_128 ble_uuid_srv_ppy;
extern const struct bt_uuid_128 ble_uuid_chr_ppy_rx;
extern const struct bt_uuid_128 ble_uuid_chr_ppy_tx;

extern const struct bt_uuid *pnt_uuid_srv_ppy;
extern const struct bt_uuid *pnt_uuid_chr_ppy_rx;
extern const struct bt_uuid *pnt_uuid_chr_ppy_tx;
