#include <stdint.h>

#include "comms.h"

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

int serialize_pet_personality_pkt(pet_personality_pkt_s *pkt, uint8_t *buffer) {

	int offset = 1;

	buffer[0] = PET_PKT_PPY_PERSONALITY;

	e_u16(pkt->id, buffer + offset);
	offset += sizeof(pkt->id);

	buffer[offset++] = pkt->sprite;

	buffer[offset++] = pkt->fav_scene;
	buffer[offset++] = pkt->fav_weather;
	buffer[offset++] = pkt->fav_time;
	buffer[offset++] = pkt->fav_temp;

	buffer[offset++] = pkt->fav_food;
	buffer[offset++] = pkt->fav_drink;

	for (int i = 0; i < PET_ATTR_NEG_MAX_D; i++) {
		buffer[offset++] = pkt->weights[i];
	}

	return offset;
}

bool deserialize_pet_personality_pkt(pet_personality_pkt_s *pkt, uint8_t *buffer) {

	int offset = 1;

	pkt->id = d_u16(buffer + offset);
	offset += sizeof(pkt->id);

	pkt->sprite = buffer[offset++];

	pkt->fav_scene = buffer[offset++];
	pkt->fav_weather = buffer[offset++];
	pkt->fav_time = buffer[offset++];
	pkt->fav_temp = buffer[offset++];

	pkt->fav_food = buffer[offset++];
	pkt->fav_drink = buffer[offset++];

	for (int i = 0; i < PET_ATTR_NEG_MAX_D; i++) {
		pkt->weights[i] = buffer[offset++];
	}

	// NOTE: these are bools in case we add
	// a validity check later on
	return true;
}

int serialize_pet_exchange_state_pkt(pet_exchange_state_pkt_s *pkt, uint8_t *buffer) {

	int offset = 1;

	buffer[0] = PET_PKT_PEX_STATE;

	buffer[offset++] = pkt->scene;
	buffer[offset++] = pkt->scene_weather;
	buffer[offset++] = pkt->scene_mood;
	buffer[offset++] = pkt->scene_time;
	buffer[offset++] = pkt->scene_temp;

	buffer[offset++] = pkt->held_food;
	buffer[offset++] = pkt->held_drink;

	return offset;
}

bool deserialize_pet_exchange_state_pkt(pet_exchange_state_pkt_s *pkt, uint8_t *buffer) {

	int offset = 1;

	pkt->scene = buffer[offset++];
	pkt->scene_weather = buffer[offset++];
	pkt->scene_mood = buffer[offset++];
	pkt->scene_time = buffer[offset++];
	pkt->scene_temp = buffer[offset++];

	pkt->held_food = buffer[offset++];
	pkt->held_drink = buffer[offset++];

	return true;
}

int serialize_pet_exchange_journal_evt_pkt(pet_exchange_journal_evt_pkt_s *pkt, uint8_t *buffer) {

	int offset = 1;

	buffer[0] = PET_PKT_PEX_JOURNAL_EVT;

	e_u16(pkt->id, buffer + offset);
	offset += sizeof(pkt->id);

	buffer[offset++] = pkt->index;

	e_u16(pkt->entry.timestamp, buffer + offset);
	offset += sizeof(pkt->entry.timestamp);

	buffer[offset++] = pkt->entry.event;

	return offset;
}

bool deserialize_pet_exchange_journal_evt_pkt(pet_exchange_journal_evt_pkt_s *pkt, uint8_t *buffer) {

	int offset = 1;

	pkt->id = d_u16(buffer + offset);
	offset += sizeof(pkt->id);

	pkt->index = buffer[offset++];

	pkt->entry.timestamp = d_u16(buffer + offset);
	offset += sizeof(pkt->entry.timestamp);

	pkt->entry.event = buffer[offset];

	return true;
}

// serialize not needed for WFC since it's one way

bool deserialize_pet_wfc_demo_cmd_pkt(pet_wfc_pkt_demo_cmd_s *pkt, uint8_t *buffer) {

	pkt->cmd_id = buffer[1];
	pkt->cmd_arg = d_u16(buffer + 2);

	return true;
}

int serialize_pet_wfc_rtc_pkt(pet_wfc_rtc_pkt_s *pkt, uint8_t *buffer) {
	int offset = 1;

	buffer[0] = PET_PKT_WFC_RTC_UPDATE;

	buffer[offset++] = pkt->secs;
	buffer[offset++] = pkt->mins;
	buffer[offset++] = pkt->hrs;

	buffer[offset++] = pkt->day;
	buffer[offset++] = pkt->month;
	buffer[offset++] = pkt->year;

	return offset;
}

bool deserialize_pet_wfc_rtc_pkt(pet_wfc_rtc_pkt_s *pkt, uint8_t *buffer) {

	int offset = 1;

	pkt->secs = buffer[offset++];
	pkt->mins = buffer[offset++];
	pkt->hrs = buffer[offset++];

	pkt->day = buffer[offset++];
	pkt->month = buffer[offset++];
	pkt->year = buffer[offset++];

	return true;
}

bool deserialize_pet_wfc_weather_pkt(pet_wfc_weather_pkt_s *pkt, uint8_t *buffer) {

	int offset = 1;

	pkt->temp_c = buffer[offset];
	pkt->weather = buffer[offset];

	return true;
}

int serialize_pet_uart_srvc_rssi_pkt(pet_uart_srvc_rssi_pkt_s *pkt, uint8_t *buffer) {

	int offset = 1;

	buffer[0] = PET_PKT_UART_RSSI;

	buffer[offset++] = pkt->rssi;

	e_u16(pkt->id, buffer + offset);
	offset += sizeof(pkt->id);

	buffer[offset++] = pkt->sprite;

	offset += serialize_pet_exchange_state_pkt(&pkt->pex_state, buffer + offset);

	return offset;
}

bool deserialize_pet_uart_srvc_rssi_pkt(pet_uart_srvc_rssi_pkt_s *pkt, uint8_t *buffer) {

	int offset = 1;

	pkt->rssi = buffer[offset++];

	pkt->id = d_u16(buffer + offset);
	offset += sizeof(pkt->id);

	pkt->sprite = buffer[offset++];

	deserialize_pet_exchange_state_pkt(&pkt->pex_state, buffer + offset);

	return true;
}
