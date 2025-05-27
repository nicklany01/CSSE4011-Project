#include "journal.h"

journal_entry_s journal[JOURNAL_MAX_ENTRIES];
journal_entry_s journal_partner[JOURNAL_MAX_ENTRIES];

int journal_idx = 0;
int journal_idx_partner = 0;

void journal_dupe_entry(journal_entry_s *dst, journal_entry_s *src) {

	dst->timestamp = src->timestamp;
	dst->event = src->event;
}

void journal_partner_add_entry(journal_entry_s *entry) {

	journal_dupe_entry(&journal_partner[journal_idx_partner], entry);
	journal_idx_partner = (journal_idx_partner + 1) % JOURNAL_MAX_ENTRIES;
}

void journal_add_entry(journal_event_e event) {

	journal[journal_idx].timestamp = 0xDEAD;
	journal[journal_idx].event = event;

	journal_idx = (journal_idx + 1) % JOURNAL_MAX_ENTRIES;
}

journal_entry_s *journal_get_entry(int idx) {

	return idx < journal_idx
		? &journal[idx]
		: NULL;
}
