#ifndef JOURNAL_H
#define JOURNAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "friends.h"

#define JOURNAL_MAX_ENTRIES 64
#define JOURNAL_FINISHED_MAGIC_NUM 96
#define JOURNAL_REQUEST_MAGIC_NUM 127
#define JOURNAL_PARTNERS_REQUEST_MAGIC_NUM 97

#define JOURNAL_MAX_PARTNERS 6

typedef enum {

	/* === STATE BASED === */
	JOURNAL_EVT_WAKE, // woke up
	JOURNAL_EVT_SLEEP, // fell asleep

	/* === SENSOR BASED === */
	JOURNAL_EVT_SHAKE, // got shook
	JOURNAL_EVT_HOT, // got hot
	JOURNAL_EVT_COLD, // got cold

	/* === ITEM BASED === */
	JOURNAL_EVT_EAT, // ate something
	JOURNAL_EVT_DRINK, // drank something

	/* === LOCATION BASED === */
	JOURNAL_EVT_BEACH, // went to the beach
	JOURNAL_EVT_MEADOW, // went to the meadow
	JOURNAL_EVT_FOREST, // went to the forest
	JOURNAL_EVT_CITY, // went to the city
	JOURNAL_EVT_SHOP, // went to the shop

	/* === COMMS BASED === */
	JOURNAL_EVT_VISIT, // visited someone
	JOURNAL_EVT_PROX_FRIEND, // came near friend
	JOURNAL_EVT_PROX_ENEMY, // came near enemy
} journal_event_e;

typedef struct {

	uint16_t timestamp;
	journal_event_e event;

} journal_entry_s;

extern journal_entry_s journal[JOURNAL_MAX_ENTRIES];
extern journal_entry_s journal_partner[JOURNAL_MAX_PARTNERS][JOURNAL_MAX_ENTRIES];

extern int journal_idx;
extern int journal_idx_partner;

extern pex_uuid_t journal_partner_lut[JOURNAL_MAX_PARTNERS];
extern int journal_partner_idx_lut[JOURNAL_MAX_PARTNERS];

void journal_purge();
int journal_partner_find_idx(pex_uuid_t partner);
void journal_partner_alloc(pex_uuid_t partner);
void journal_dupe_entry(journal_entry_s *dst, journal_entry_s *src);
void journal_partner_add_entry(journal_entry_s *entry);
void journal_add_entry(journal_event_e event, uint16_t timestamp);

#endif
