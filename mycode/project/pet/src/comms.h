#ifndef COMMS_H
#define COMMS_H

#include <stdint.h>

#include "scenes.h"
#include "personality.h"
#include "journal.h"

#define SIENNA_MF_ID 0x4369

#define BLE_SERV_UUID16_PPY 0x4369
#define BLE_SERV_UUID16_PEX 0xCAFE
#define BLE_SERV_UUID16_WFC 0xFEED

typedef struct {

	pex_uuid_t id;

	sprite_s sprite;

	main_scenes_e fav_scene;
	mod_weather_e fav_weather;
	mod_time_e fav_time;
	mod_temp_e fav_temp;

	foods_e fav_food;
	drinks_e fav_drink;

	uint16_t weights[PET_ATTR_NEG_MAX];
} pet_personality_pkt_s;

typedef struct {

	main_scenes_e scene;
	mod_weather_e scene_weather;
	mod_mood_e scene_mood;
	mod_time_e scene_time;
	mod_temp_e scene_temp;

	foods_e held_food;
	drinks_e held_drink;

} pet_exchange_state_pkt_s;

typedef struct {

	journal_entry_s journal[JOURNAL_MAX_ENTRIES];
} pet_exchange_journal_pkt_s;

typedef struct {


} pet_wfc_pkt_s;

void serialize_pet_personality_pkt(pet_personality_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_personality_pkt(pet_personality_pkt_s *pkt, uint8_t *buffer);

void serialize_pet_exchange_state_pkt(pet_exchange_state_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_exchange_state_pkt(pet_exchange_state_pkt_s *pkt, uint8_t *buffer);

void serialize_pet_exchange_journal_pkt(pet_exchange_journal_pkt_s *pkt, uint8_t *buffer);
bool deserialize_pet_exchange_journal_pkt(pet_exchange_journal_pkt_s *pkt, uint8_t *buffer);

#endif
