#ifndef JOURNAL_H
#define JOURNAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define JOURNAL_MAX_ENTRIES 64

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

	uint16_t time_stamp;
	journal_event_e event;

} journal_entry_s;

extern journal_entry_s journal[JOURNAL_MAX_ENTRIES];

#endif
