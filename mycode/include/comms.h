#ifndef COMMS_H
#define COMMS_H

#include <stdint.h>

#include "friends.h"
#include "journal.h"

#define SIENNA_MF_ID 0x4369

#define BLE_SERV_UUID16_PPY 0x4369
#define BLE_SERV_UUID16_PEX 0xCAFE
#define BLE_SERV_UUID16_WFC 0xFEED

#define PET_ATTR_NEG_MAX_D 9

#define BLE_ADV_PEX_STATE_START 5

typedef enum {

	BLE_ADV_MF_ID_HIGH,
	BLE_ADV_MF_ID_LOW,

	BLE_ADV_PEX_ID_HIGH,
	BLE_ADV_PEX_ID_LOW,
	BLE_ADV_MY_SPRITE,
} ble_adv_pkt_pos_e;


typedef enum {

	PET_PKT_PPY_PERSONALITY,

	PET_PKT_PEX_STATE,
	PET_PKT_PEX_JOURNAL,
	PET_PKT_PEX_JOURNAL_EVT,

	PET_PKT_WFC_DEMO_COMMAND,
	PET_PKT_WFC_RTC_UPDATE,
	PET_PKT_WFC_WEATHER_UPDATE,

	// NOTE: keep UART packets LAST
	PET_PKT_UART_RSSI
} pet_pkt_id_e;

typedef struct {

	pex_uuid_t id;

	uint8_t sprite;

	uint8_t fav_scene;
	uint8_t fav_weather;
	uint8_t fav_time;
	uint8_t fav_temp;

	uint8_t fav_food;
	uint8_t fav_drink;

	uint16_t weights[PET_ATTR_NEG_MAX_D];
} pet_personality_pkt_s;

typedef struct {

	uint8_t scene;
	uint8_t scene_weather;
	uint8_t scene_mood;
	uint8_t scene_time;
	uint8_t scene_temp;

	uint8_t held_food;
	uint8_t held_drink;

} pet_exchange_state_pkt_s;

typedef struct {

	uint8_t index;
	journal_entry_s entry;

} pet_exchange_journal_evt_pkt_s;

typedef enum {

	PET_WFC_CMD_CHANGE_SCENE,
	PET_WFC_CMD_CHANGE_MOOD,
	PET_WFC_CMD_CHANGE_TIME,

	PET_WFC_CMD_ADD_FRIEND,
	PET_WFC_CMD_ADD_ENEMY,

	PET_WFC_CMD_REM_FRIEND,
	PET_WFC_CMD_REM_ENEMY,
} pet_wfc_demo_cmds_e;

typedef struct {

	pet_wfc_demo_cmds_e cmd_id;
	uint16_t cmd_arg;
} pet_wfc_pkt_demo_cmd_s;

typedef struct {

	uint8_t secs;
	uint8_t mins;
	uint8_t hrs;

	uint8_t day;
	uint8_t month;
	uint8_t year;
} pet_wfc_rtc_pkt_s;

typedef struct {

	int8_t temp_c;
	uint8_t weather;
} pet_wfc_weather_pkt_s;

typedef struct {

	int8_t rssi;

	pex_uuid_t id;
	uint8_t sprite;

	pet_exchange_state_pkt_s pex_state;
} pet_uart_srvc_rssi_pkt_s;

uint16_t d_u16(uint8_t *buffer);

int serialize_pet_personality_pkt(pet_personality_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_personality_pkt(pet_personality_pkt_s *pkt, uint8_t *buffer);

int serialize_pet_exchange_state_pkt(pet_exchange_state_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_exchange_state_pkt(pet_exchange_state_pkt_s *pkt, uint8_t *buffer);

int serialize_pet_exchange_journal_evt_pkt(pet_exchange_journal_evt_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_exchange_journal_evt_pkt(pet_exchange_journal_evt_pkt_s *pkt, uint8_t *buffer);

bool deserialize_pet_wfc_demo_cmd_pkt(pet_wfc_pkt_demo_cmd_s *pkt, uint8_t *buffer);
bool deserialize_pet_wfc_rtc_pkt(pet_wfc_rtc_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_wfc_weather_pkt(pet_wfc_weather_pkt_s *pkt, uint8_t *buffer);

int serialize_pet_uart_srvc_rssi_pkt(pet_uart_srvc_rssi_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_uart_srvc_rssi_pkt(pet_uart_srvc_rssi_pkt_s *pkt, uint8_t *buffer);

#endif
