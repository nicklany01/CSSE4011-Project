#include "ble_uuid.h"

const struct bt_uuid_128 ble_uuid_srv_ppy = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4369, 0x4153, 0xAD28, 0xB8D61B4F447A));

// NOTE TX and RX here are from POV of me as the client
const struct bt_uuid_128 ble_uuid_chr_ppy_rx = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4770, 0x4153, 0xAD28, 0xB8D61B4F447A));

const struct bt_uuid_128 ble_uuid_chr_ppy_tx = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4771, 0x4153, 0xAD28, 0xB8D61B4F447A));

const struct bt_uuid *pnt_uuid_srv_ppy = BT_UUID_DECLARE_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4369, 0x4153, 0xAD28, 0xB8D61B4F447A));

const struct bt_uuid *pnt_uuid_chr_ppy_rx = BT_UUID_DECLARE_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4770, 0x4153, 0xAD28, 0xB8D61B4F447A));

const struct bt_uuid *pnt_uuid_chr_ppy_tx = BT_UUID_DECLARE_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4771, 0x4153, 0xAD28, 0xB8D61B4F447A));
