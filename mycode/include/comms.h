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

typedef enum {

	BLE_ADV_MF_ID_HIGH,
	BLE_ADV_MF_ID_LOW,

	BLE_ADV_PEX_ID_HIGH,
	BLE_ADV_PEX_ID_LOW,
	BLE_ADV_MY_SPRITE,

	BLE_ADV_CURR_SCENE,
	BLE_ADV_CURR_TIME,

	BLE_ADV_CURR_FOOD,
	BLE_ADV_CURR_DRINK

} ble_adv_pkt_pos_e;


typedef enum {

	PET_PKT_PPY_PERSONALITY,
	PET_PKT_PEX_STATE,
	PET_PKT_PEX_JOURNAL,
	PET_PKT_PEX_JOURNAL_EVT,
	PET_PKT_WFC_DEMO_COMMAND
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

	journal_entry_s journal[JOURNAL_MAX_ENTRIES];
} pet_exchange_journal_pkt_s;

typedef struct {


} pet_exchange_journal_evt_pkt_s;

typedef enum {

	PET_WFC_CMD_CHANGE_SCENE,
	PET_WFC_CMD_CHANGE_MOOD,
	PET_WFC_CMD_CHANGE_TIME
} pet_wfc_demo_cmds_e;

typedef struct {

	pet_wfc_demo_cmds_e cmd_id;
	uint16_t cmd_arg;
} pet_wfc_pkt_demo_cmd_s;

int serialize_pet_personality_pkt(pet_personality_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_personality_pkt(pet_personality_pkt_s *pkt, uint8_t *buffer);

int serialize_pet_exchange_state_pkt(pet_exchange_state_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_exchange_state_pkt(pet_exchange_state_pkt_s *pkt, uint8_t *buffer);

void serialize_pet_exchange_journal_pkt(pet_exchange_journal_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_exchange_journal_pkt(pet_exchange_journal_pkt_s *pkt, uint8_t *buffer);

int serialize_pet_wfc_demo_cmd_pkt(pet_wfc_pkt_demo_cmd_s *pkt, uint8_t *buffer);
bool deserialize_pet_wfc_demo_cmd_pkt(pet_wfc_pkt_demo_cmd_s *pkt, uint8_t *buffer);

#endif
