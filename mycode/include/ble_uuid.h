#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#define BLE_UUID_16_SRV_PPY 0x4369
#define BLE_UUID_16_CHR_PPY_RX 0x4770
#define BLE_UUID_16_CHR_PPY_TX 0x4771

#define BLE_UUID_16_SRV_PEX 0xCAFE
#define BLE_UUID_16_CHR_PEX_RX 0x4772
#define BLE_UUID_16_CHR_PEX_TX 0x4773

#define BLE_UUID_16_SRV_WFC 0xFEED
#define BLE_UUID_16_CHR_WFC_RX 0x4774
#define BLE_UUID_16_CHR_WFC_TX 0x4775

extern const struct bt_uuid_128 ble_uuid_srv_ppy;
extern const struct bt_uuid_128 ble_uuid_chr_ppy_rx;
extern const struct bt_uuid_128 ble_uuid_chr_ppy_tx;

extern const struct bt_uuid_128 ble_uuid_srv_pex;
extern const struct bt_uuid_128 ble_uuid_chr_pex_rx;
extern const struct bt_uuid_128 ble_uuid_chr_pex_tx;

extern const struct bt_uuid_128 ble_uuid_srv_wfc;
extern const struct bt_uuid_128 ble_uuid_chr_wfc_rx;
extern const struct bt_uuid_128 ble_uuid_chr_wfc_tx;

