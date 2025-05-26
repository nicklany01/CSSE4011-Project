#include <stdbool.h>
#include <string.h>

#include "os_ble.h"
#include "ble_uuid.h"
#include <zephyr/bluetooth/gap.h>

uint8_t os_ble_evt_flags = 0x00;
int count = 0;

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

// NOTE: TX and RX here are from POV of me as the client
uint16_t srv_ppy_tx_data;
uint16_t srv_ppy_rx_data;

struct bt_conn *upl_conn;
struct bt_conn *bae_conn;

struct bt_le_conn_param *conn_params = BT_LE_CONN_PARAM(10, 100, 0, 1000); // 30ms–50ms, no latency, 4s timeout

static struct bt_gatt_discover_params upl_params;
static struct bt_gatt_subscribe_params upl_subscriptions;

static bool os_ble_is_sienna(struct bt_data *data, void *user_data);

static uint8_t os_ble_gatt_srv_discover(struct bt_conn *conn,
	const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params) {

	count++;

	if (count > 1) {


	}

	if (attr == NULL) {
		return BT_GATT_ITER_CONTINUE;
	}

	if (!bt_uuid_cmp(attr->uuid, pnt_uuid_srv_ppy)) {

		upl_params.uuid = &ble_uuid_chr_ppy_rx.uuid;
		upl_params.start_handle = attr->handle + 1;
		upl_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

		bt_gatt_discover(conn, &upl_params);
	} else if (!bt_uuid_cmp(attr->uuid, pnt_uuid_chr_ppy_rx)) {


	} else {

	}

	return BT_GATT_ITER_CONTINUE;
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

	bt_data_parse(ad, os_ble_is_sienna, (void *)addr);

}

static struct k_work_delayable discover_work;

static void os_ble_connected(struct bt_conn *connected, uint8_t err) {

	if (connected == upl_conn) {
		upl_params.uuid = NULL;
		upl_params.func = os_ble_gatt_srv_discover;
		upl_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
		upl_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
		upl_params.type = BT_GATT_DISCOVER_PRIMARY;

		bt_gatt_discover(upl_conn, &upl_params);
	} else {
		os_ble_evt_flags |= OS_BLE_EVT_MSK_BASE;
	}
}

static void os_ble_disconnected(struct bt_conn *disconn, uint8_t reason) {

	os_ble_evt_flags |= OS_BLE_EVT_MSK_UPLINK;
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = os_ble_connected,
	.disconnected = os_ble_disconnected,
};


static bool os_ble_is_sienna(struct bt_data *data, void *user_data) {

	char localname[30];
	int len;
	struct bt_conn *target = NULL;

	bt_addr_le_t *addr = user_data;

	switch (data->type) {
		case BT_DATA_MANUFACTURER_DATA:
			break;

		case BT_DATA_NAME_SHORTENED:
		case BT_DATA_NAME_COMPLETE:

			len = MIN(data->data_len, 29);
			memcpy(localname, data->data, len);
			localname[len] = '\0';

			if (!strncmp(localname, BLE_LOCAL_NAME_UPLINK, len)) {

				os_ble_evt_flags |= OS_BLE_EVT_MSK_ADV_RX;

				bt_le_scan_stop();
				bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, conn_params, &upl_conn);
				return false;
			} else if (!strncmp(localname, BLE_LOCAL_NAME_BASE, len)) {

				bt_le_scan_stop();
				bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, conn_params, &bae_conn);
				return false;
			}

			break;

		default:
			break;
	}

	return true;
}

bool os_ble_init() {

	if (bt_enable(NULL)) {
		return false;
	}

	bt_le_scan_stop();

	int err;

	err = bt_le_scan_start(BT_LE_SCAN_ACTIVE, os_ble_adv_rx);

	if (err) {
		return false;
	}

	return true;
}

