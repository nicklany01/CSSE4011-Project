#include <string.h>

#include "journal.h"
#include "friends.h"

journal_entry_s journal[JOURNAL_MAX_ENTRIES];
journal_entry_s journal_partner[JOURNAL_MAX_PARTNERS][JOURNAL_MAX_ENTRIES];

pex_uuid_t journal_partner_lut[JOURNAL_MAX_PARTNERS] = {0};
int journal_partner_idx_lut[JOURNAL_MAX_PARTNERS] = {0};

int journal_idx = 0;

pex_uuid_t current_partner = 0;
int current_partner_idx = -1;
int journal_idx_partner = 0;

void journal_purge() {

	journal_idx = 0;
	current_partner = 0;
	current_partner_idx = -1;

	memset(journal, 0x00, sizeof(journal));
	memset(journal_partner, 0x00, sizeof(journal_partner));

	memset(journal_partner_lut, 0x00, sizeof(journal_partner_lut));
	memset(journal_partner_idx_lut, 0x00, sizeof(journal_partner_idx_lut));
}

int journal_partner_find_idx(pex_uuid_t partner) {

	for (int i = 0; i < JOURNAL_MAX_PARTNERS; i++) {
		if (journal_partner_lut[i] == partner) {
			return i;
		}
	}

	return -1;
}

void journal_partner_alloc(pex_uuid_t partner) {

	int existing = journal_partner_find_idx(partner);

	if (existing != -1) {

		current_partner = partner;
		current_partner_idx = existing;
		return;
	}

	int free = journal_partner_find_idx(0x0000);
	if (free == -1) {
		// wrap around, kill earliest
		journal_idx_partner = JOURNAL_MAX_PARTNERS;
		free = 0;
	} else {
		journal_idx_partner += 1;
	}

	current_partner = partner;
	current_partner_idx = free;

	journal_partner_lut[current_partner_idx] = partner;
	memset(journal_partner[current_partner_idx], 0x00, sizeof(journal_entry_s) * JOURNAL_MAX_ENTRIES);
}

void journal_dupe_entry(journal_entry_s *dst, journal_entry_s *src) {

	dst->timestamp = src->timestamp;
	dst->event = src->event;
}

void journal_partner_add_entry(journal_entry_s *entry) {

	int next_entry = journal_partner_idx_lut[current_partner_idx];

	journal_dupe_entry(&journal_partner[current_partner_idx][next_entry], entry);
	journal_partner_idx_lut[current_partner_idx] = (next_entry + 1) % JOURNAL_MAX_ENTRIES;
}

void journal_add_entry(journal_event_e event, uint16_t timestamp) {

	journal[journal_idx].timestamp = timestamp;
	journal[journal_idx].event = event;

	journal_idx = (journal_idx + 1) % JOURNAL_MAX_ENTRIES;
}
