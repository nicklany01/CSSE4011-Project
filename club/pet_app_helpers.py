# WARNING: keep sync'd with comms.h
class PET_WFC_DEMO_CMDS:
	CHANGE_SCENE = 0
	CHANGE_MOOD = 1
	CHANGE_TIME = 2

class SPRITE:
	ZERO = 0
	CHERRY = 1
	ICE = 2
	GRAPE = 3
	BAJA_BLAST = 4


class PET_PKT_ID:
	PPY_PERSONALITY = 0
	PEX_STATE = 1
	PEX_JOURNAL = 2
	PEX_JOURNAL_EVT = 3
	WFC_DEMO_COMMAND = 4
	WFC_RTC_UPDATE = 5
	WFC_WEATHER_UPDATE = 6

class PET_BLE_ADV_POS:
	PEX_ID_HIGH = 0
	PEX_ID_LOW = 1
	MY_SPRITE = 2

	CURR_SCENE = 3
	CURR_TIME = 4

	CURR_FOOD = 5
	CURR_DRINK = 6

class PET_JOURNAL_EVENTS:
	JOURNAL_EVT_WAKE = 0 # woke up
	JOURNAL_EVT_SLEEP = 1 # fell asleep

	# === SENSOR BASED ===
	JOURNAL_EVT_SHAKE = 2 # got shook
	JOURNAL_EVT_HOT = 3 # got hot
	JOURNAL_EVT_COLD = 4 # got cold

	# === ITEM BASED ===
	JOURNAL_EVT_EAT = 5 # ate something
	JOURNAL_EVT_DRINK = 6 # drank something

	# === LOCATION BASED ===
	JOURNAL_EVT_BEACH = 7 # went to the beach
	JOURNAL_EVT_MEADOW = 8 # went to the meadow
	JOURNAL_EVT_FOREST = 9 # went to the forest
	JOURNAL_EVT_CITY = 10 # went to the city
	JOURNAL_EVT_SHOP = 11 # went to the shop

	# === COMMS BASED ===
	JOURNAL_EVT_VISIT = 12 # visited someone
	JOURNAL_EVT_PROX_FRIEND = 13 # came near friend
	JOURNAL_EVT_PROX_ENEMY = 14 # came near enemy

JOURNAL_MAX_ENTRIES = 64

JOURNAL_FRIENDLY_STRINGS = {
	PET_JOURNAL_EVENTS.JOURNAL_EVT_WAKE: "woke up!",
	PET_JOURNAL_EVENTS.JOURNAL_EVT_SLEEP: "fell asleep...",

	PET_JOURNAL_EVENTS.JOURNAL_EVT_SHAKE: "got shook up :\\",
	PET_JOURNAL_EVENTS.JOURNAL_EVT_HOT: "felt pretty hot!!",
	PET_JOURNAL_EVENTS.JOURNAL_EVT_COLD: "felt chilly",

	PET_JOURNAL_EVENTS.JOURNAL_EVT_EAT: "ate somethin!",
	PET_JOURNAL_EVENTS.JOURNAL_EVT_DRINK: "quenched my thirst",

	PET_JOURNAL_EVENTS.JOURNAL_EVT_BEACH: "hit the beach",
	PET_JOURNAL_EVENTS.JOURNAL_EVT_MEADOW: "frollicked in the meadow",
	PET_JOURNAL_EVENTS.JOURNAL_EVT_FOREST: "explored the forest",
	PET_JOURNAL_EVENTS.JOURNAL_EVT_CITY: "became a little guy in the big city",
	PET_JOURNAL_EVENTS.JOURNAL_EVT_SHOP: "went shoppin",

	PET_JOURNAL_EVENTS.JOURNAL_EVT_VISIT: "had a visitor!",
	PET_JOURNAL_EVENTS.JOURNAL_EVT_PROX_FRIEND: "passed by my friend :D",
	PET_JOURNAL_EVENTS.JOURNAL_EVT_PROX_ENEMY: "saw an opp >:("
}

class PetJournalEntry:
	def __init__(self):

		self.timestamp = 0
		self.event = PET_JOURNAL_EVENTS.JOURNAL_EVT_WAKE

	def __str__(self):

		return JOURNAL_FRIENDLY_STRINGS[self.event]

class PetJournal:

	def __init__(self):

		self.entries = [None for i in range(0, JOURNAL_MAX_ENTRIES)]
		self.index = 0

	def add(self, entry):

		added_at = self.index
		self.entries[self.index] = entry

		self.index = (self.index + 1) % JOURNAL_MAX_ENTRIES

		return added_at

	def get(self, index):
		return self.entries[index % JOURNAL_MAX_ENTRIES]
