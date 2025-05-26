#include "journal.h"

journal_entry_s journal[JOURNAL_MAX_ENTRIES];

static int journal_idx = 0;

bool journal_add_entry(journal_event_e event) {

	if (journal_idx == JOURNAL_MAX_ENTRIES) {
		return false;
	}

	journal[journal_idx].time_stamp = 0xDEAD;
	journal[journal_idx].event = event;

	journal_idx++;
	return true;
}

journal_entry_s *journal_get_entry(int idx) {

	return idx < journal_idx
		? &journal[idx]
		: NULL;
}
