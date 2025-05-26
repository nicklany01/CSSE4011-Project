#include <stdint.h>
#include <string.h>

#include "os_ble.h"


struct k_msgq os_ble_rxq;

uint8_t os_ble_rx_buff[RX_BUFF_SIZE] = {0};
char os_ble_rxq_buff[RX_QUEUE_SIZE * sizeof(os_ble_passthru_s)];

static const struct bt_uuid_128 ble_uuid_srv_ppy = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4369, 0x4153, 0xAD28, 0xB8D61B4F447A));

// NOTE TX and RX here are from POV of me as the client
static const struct bt_uuid_128 ble_uuid_chr_ppy_rx = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4770, 0x4153, 0xAD28, 0xB8D61B4F447A));

static const struct bt_uuid_128 ble_uuid_chr_ppy_tx = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x4A259CE4, 0x4771, 0x4153, 0xAD28, 0xB8D61B4F447A));

// NOTE: TX and RX here are from POV of me as the server
uint16_t srv_ppy_tx_data;
uint16_t srv_ppy_rx_data;

uint8_t mf_data[MF_DLEN] = {

	MF_ID_HIGH,
	MF_ID_LOW,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

struct bt_conn *pn_conn;
struct bt_conn *bn_conn;

void os_ble_update_mf_data(uint8_t *new_mf_data) {

	memcpy((uint8_t *)mf_data, new_mf_data, MF_DLEN);
}

int os_ble_start_advertising() {

	struct bt_data adv_pkt[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
		BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
		BT_DATA(BT_DATA_MANUFACTURER_DATA, mf_data, MF_DLEN),
		//BT_DATA_BYTES(BT_DATA_UUID128_SOME, &ble_uuid_srv_ppy.uuid)
	};

	return bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, adv_pkt, ARRAY_SIZE(adv_pkt), NULL, 0);
}

int os_ble_restart_advertising() {
	bt_le_adv_stop();

	return os_ble_start_advertising();
}

static void os_ble_cccd_update(const struct bt_gatt_attr *attr, uint16_t val) {
	if (val == BT_GATT_CCC_NOTIFY) {
		// other pet subbed for notifs we should do something here
	}
}

static ssize_t os_ble_ppy_tx_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
	const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {

	os_ble_passthru_s passthru_rx;

	passthru_rx.len = len;
	memcpy(passthru_rx.buff, buf, len);

	k_msgq_put(&os_ble_rxq, &passthru_rx, K_NO_WAIT);
}

BT_GATT_SERVICE_DEFINE(srv_ppy,
	BT_GATT_PRIMARY_SERVICE(&ble_uuid_srv_ppy.uuid),
	BT_GATT_CHARACTERISTIC(&ble_uuid_chr_ppy_rx.uuid,
		BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, &srv_ppy_tx_data),
	BT_GATT_CCC(os_ble_cccd_update, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(&ble_uuid_chr_ppy_tx.uuid,
		BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, os_ble_ppy_tx_cb, (void *)1),
);

static void os_ble_connected(struct bt_conn *connected, uint8_t err) {

}

static void os_ble_disconnected(struct bt_conn *disconn, uint8_t reason) {

	os_ble_restart_advertising();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = os_ble_connected,
	.disconnected = os_ble_disconnected,
};


bool os_ble_init() {

	k_msgq_init(&os_ble_rxq, os_ble_rxq_buff, sizeof(os_ble_passthru_s), RX_QUEUE_SIZE);

	if (bt_enable(NULL)) {
		return false;
	}

	bt_le_adv_stop();

	int err;

	err = os_ble_start_advertising();

	if (err) {
		return false;
	}

	return true;
}

