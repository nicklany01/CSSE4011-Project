#include "ble_uuid.h"

// <--- PPY SERVICE --->
const struct bt_uuid_128 ble_uuid_srv_ppy = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4369, 0x4153, 0xAD28, 0xB8D61B4F447A));

// NOTE TX and RX here are from POV of me as the client
const struct bt_uuid_128 ble_uuid_chr_ppy_rx = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4770, 0x4153, 0xAD28, 0xB8D61B4F447A));

const struct bt_uuid_128 ble_uuid_chr_ppy_tx = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4771, 0x4153, 0xAD28, 0xB8D61B4F447A));


// <--- PEX SERVICE --->
const struct bt_uuid_128 ble_uuid_srv_pex = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0xCAFE, 0x4153, 0xAD28, 0xB8D61B4F447A));

// NOTE TX and RX here are from POV of me as the client
const struct bt_uuid_128 ble_uuid_chr_pex_rx = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4772, 0x4153, 0xAD28, 0xB8D61B4F447A));

const struct bt_uuid_128 ble_uuid_chr_pex_tx = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4773, 0x4153, 0xAD28, 0xB8D61B4F447A));


// <--- WFC SERVICE --->
const struct bt_uuid_128 ble_uuid_srv_wfc = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0xFEED, 0x4153, 0xAD28, 0xB8D61B4F447A));

// NOTE TX and RX here are from POV of me as the client
const struct bt_uuid_128 ble_uuid_chr_wfc_rx = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4774, 0x4153, 0xAD28, 0xB8D61B4F447A));

const struct bt_uuid_128 ble_uuid_chr_wfc_tx = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4775, 0x4153, 0xAD28, 0xB8D61B4F447A));
