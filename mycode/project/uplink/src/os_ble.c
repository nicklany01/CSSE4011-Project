#include <stdint.h>
#include <string.h>

#include "os_ble.h"
#include "ble_uuid.h"

struct k_msgq os_ble_rxq;
struct k_msgq os_ble_advq;

char os_ble_rxq_buff[RX_QUEUE_SIZE * sizeof(os_ble_passthru_s)];
char os_ble_advq_buff[RX_QUEUE_SIZE * sizeof(os_ble_pet_adv_s)];

// NOTE: TX and RX here are from POV of me as the server
uint8_t srv_ppy_tx_data[RX_BUFF_SIZE] = {0};
uint8_t srv_pex_tx_data[RX_BUFF_SIZE] = {0};
uint8_t srv_wfc_tx_data[RX_BUFF_SIZE] = {0};

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


struct bt_le_scan_param params = {

	.type = BT_LE_SCAN_TYPE_PASSIVE,
	.options = BT_LE_SCAN_OPT_CODED,
	.interval = BT_GAP_SCAN_FAST_INTERVAL,
	.window = BT_GAP_SCAN_FAST_WINDOW,
};

struct bt_conn *pn_conn;
struct bt_conn *bn_conn;

int8_t last_rssi = 0;

os_ble_state_s os_ble_state = {

	.state = OS_BLE_STATE_UNINIT
};

void os_ble_update_mf_data(uint8_t *new_mf_data) {

	memcpy((uint8_t *)mf_data, new_mf_data, MF_DLEN);
}

static bool os_ble_is_sienna(struct bt_data *data, void *user_data) {

	struct bt_conn *target = NULL;
	uint16_t company_id;
	os_ble_pet_adv_s pet_adv;

	bt_addr_le_t *addr = user_data;

	switch (data->type) {
		case BT_DATA_MANUFACTURER_DATA:

			company_id = (uint16_t)data->data[0] << 8 | data->data[1];
			if (company_id == SIENNA_MF_ID) {

				memcpy(pet_adv.mf_data, data->data, data->data_len);
				pet_adv.rssi = last_rssi;

				k_msgq_put(&os_ble_advq, &pet_adv, K_NO_WAIT);

				return false;
			}

			break;
		default:
			break;
	}

	return true;
}

static void os_ble_adv_rx(const bt_addr_le_t *addr, int8_t rssi,
	uint8_t type, struct net_buf_simple *ad) {


	if (type != BT_GAP_ADV_TYPE_ADV_IND &&
			type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND && type != BT_GAP_ADV_TYPE_SCAN_RSP) {

		return;
	}

	if (rssi < -80) {
		return;
	}

	last_rssi = rssi;
	bt_data_parse(ad, os_ble_is_sienna, (void *)addr);

}

void os_ble_start_scan() {
	bt_le_scan_start(&params, &os_ble_adv_rx);
}

void os_ble_stop_scan() {
	bt_le_scan_stop();
}

void os_ble_stop_advertising() {
	bt_le_adv_stop();
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

BT_GATT_SERVICE_DEFINE(srv_pex,
	BT_GATT_PRIMARY_SERVICE(&ble_uuid_srv_pex.uuid),
	BT_GATT_CHARACTERISTIC(&ble_uuid_chr_pex_rx.uuid,
		BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, &srv_pex_tx_data),
	BT_GATT_CCC(os_ble_cccd_update, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(&ble_uuid_chr_pex_tx.uuid,
		BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, os_ble_ppy_tx_cb, (void *)1),
);

BT_GATT_SERVICE_DEFINE(srv_wfc,
	BT_GATT_PRIMARY_SERVICE(&ble_uuid_srv_wfc.uuid),
	BT_GATT_CHARACTERISTIC(&ble_uuid_chr_wfc_rx.uuid,
		BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, &srv_wfc_tx_data),
	BT_GATT_CCC(os_ble_cccd_update, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(&ble_uuid_chr_wfc_tx.uuid,
		BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, os_ble_ppy_tx_cb, (void *)1),
);

void os_ble_notify(os_ble_passthru_s *passthru) {

	switch (passthru->charac) {

		case BLE_UUID_16_CHR_PPY_RX:
			bt_gatt_notify(NULL, &srv_ppy.attrs[1], passthru->buff, passthru->len);
			break;
		case BLE_UUID_16_CHR_PEX_RX:
			bt_gatt_notify(NULL, &srv_pex.attrs[1], passthru->buff, passthru->len);
			break;
		case BLE_UUID_16_CHR_WFC_RX:
			bt_gatt_notify(NULL, &srv_wfc.attrs[1], passthru->buff, passthru->len);
			break;

		default:
			break;
	}
}

static void os_ble_connected(struct bt_conn *connected, uint8_t err) {
	os_ble_state.state = OS_BLE_STATE_CONNECTED;
}

static void os_ble_disconnected(struct bt_conn *disconn, uint8_t reason) {

	os_ble_state.state = OS_BLE_STATE_ADVERTISE;
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = os_ble_connected,
	.disconnected = os_ble_disconnected,
};


bool os_ble_init() {

	k_msgq_init(&os_ble_rxq, os_ble_rxq_buff, sizeof(os_ble_passthru_s), RX_QUEUE_SIZE);
	k_msgq_init(&os_ble_advq, os_ble_advq_buff, sizeof(os_ble_pet_adv_s), RX_QUEUE_SIZE);

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

