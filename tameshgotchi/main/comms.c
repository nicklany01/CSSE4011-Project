#include "comms.h"
#include "journal.h"
#include "personality.h"

void e_u16(uint16_t val, uint8_t *buffer) {
	buffer[0] = val >> 8;
	buffer[1] = val & 0xFF;
}

uint16_t d_u16(uint8_t *buffer) {
	return ((uint16_t)buffer[0] << 8) | (buffer[1] & 0xFF);
}

void e_u32(uint32_t val, uint8_t *buffer) {
	buffer[0] = val >> 24;
	buffer[1] = val >> 16;
	buffer[2] = val >> 8;
	buffer[3] = val & 0xFF;
}

uint32_t d_u32(uint8_t *buffer) {
	return ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) | ((uint32_t)buffer[0] << 8) | (buffer[1] & 0xFF);
}

void serialize_pet_personality_pkt(pet_personality_pkt_s *pkt, uint8_t *buffer) {

	int offset = sizeof(pkt->id);

	e_u16(pkt->id, buffer);

	buffer[offset++] = pkt->sprite;

	buffer[offset++] = pkt->fav_scene;
	buffer[offset++] = pkt->fav_weather;
	buffer[offset++] = pkt->fav_time;
	buffer[offset++] = pkt->fav_food;

	for (int i = 0; i < PET_ATTR_NEG_MAX; i++) {
		e_u16(pkt->weights[i], buffer + offset);
		offset += sizeof(uint16_t);
	}
}

bool deserialize_pet_personality_pkt(pet_personality_pkt_s *pkt, uint8_t *buffer) {

	int offset = sizeof(pkt->id);

	pkt->id = d_u16(buffer);

	pkt->sprite = buffer[offset++];

	pkt->fav_scene = buffer[offset++];
	pkt->fav_weather = buffer[offset++];
	pkt->fav_time = buffer[offset++];
	pkt->fav_food = buffer[offset++];

	for (int i = 0; i < PET_ATTR_NEG_MAX; i++) {
		pkt->weights[i] = d_u16(buffer + offset);
		offset += sizeof(uint16_t);
	}

	// NOTE: these are bools in case we add
	// a validity check later on
	return true;
}

void serialize_pet_exchange_state_pkt(pet_exchange_state_pkt_s *pkt, uint8_t *buffer) {

	int offset = 0;

	buffer[offset++] = pkt->scene;
	buffer[offset++] = pkt->scene_weather;
	buffer[offset++] = pkt->scene_mood;
	buffer[offset++] = pkt->scene_time;

	buffer[offset++] = pkt->held_food;
	buffer[offset++] = pkt->held_drink;

}

bool deserialize_pet_exchange_state_pkt(pet_exchange_state_pkt_s *pkt, uint8_t *buffer) {

	int offset = 0;

	pkt->scene = buffer[offset++];
	pkt->scene_weather = buffer[offset++];
	pkt->scene_mood = buffer[offset++];
	pkt->scene_time = buffer[offset++];

	pkt->held_food = buffer[offset++];
	pkt->held_drink = buffer[offset++];

	return true;
}

void serialize_pet_exchange_journal_pkt(pet_exchange_journal_pkt_s *pkt, uint8_t *buffer) {

	int offset = 0;

	for (int i = 0; i < JOURNAL_MAX_ENTRIES; i++) {

		e_u16(pkt->journal[i].time_stamp, buffer + offset);
		offset += sizeof(uint16_t);

		buffer[offset++] = pkt->journal[i].event;
	}
}

bool deserialize_pet_exchange_journal_pkt(pet_exchange_journal_pkt_s *pkt, uint8_t *buffer) {

	int offset = 0;

	for (int i = 0; i < JOURNAL_MAX_ENTRIES; i++) {

		pkt->journal[i].time_stamp = d_u16(buffer + offset);
		offset += sizeof(uint16_t);

		pkt->journal[i].event = buffer[offset++];
	}

	return true;
}





