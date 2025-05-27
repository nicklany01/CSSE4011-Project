from enum import Enum

class JournalEvents(Enum):
    # === STATE BASED ===
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


class Food(Enum):
	FOOD_NONE = 0
	FOOD_PIZZA = 1
	FOOD_BURGER = 2
	FOOD_KFC = 3
	FOOD_SUSHI = 4
	FOOD_TACOS = 5
	FOOD_HOT_DOGS = 6
	FOOD_PASTA = 7
	FOOD_WINGS = 8


class Drink(Enum):
	DRINK_NONE = 0
	DRINK_COLA = 1
	DRINK_LEMONADE = 2
	DRINK_MOUNTAIN_DEW = 3
	DRINK_WATER = 4
	DRINK_JUICE = 5
	DRINK_KOMBUCHA = 6
	DRINK_TEA = 7
	DRINK_COFFEE = 8
	DRINK_BEER = 9


class Sprite(Enum):
	SPRITE_ZERO = 0
	SPRITE_CHERRY = 1
	SPRITE_ICE = 2
	SPRITE_GRAPE = 3
	SPRITE_BAJA_BLAST = 4

class Scene(Enum):
	MAIN_SCENE_MEADOW = 0
	MAIN_SCENE_BEACH = 1
	MAIN_SCENE_FOREST = 2
	MAIN_SCENE_CITY = 3
	MAIN_SCENE_SHOP = 4

class Weather(Enum):
	MOD_WEATHER_SUNNY = 0
	MOD_WEATHER_RAINY = 1
	MOD_WEATHER_CLOUDY = 2
	MOD_WEATHER_SNOWY = 3

class Mood(Enum):
	MOD_MOOD_NEUTRAL = 0
	MOD_MOOD_HAPPY = 1
	MOD_MOOD_SAD = 2
	MOD_MOOD_ANGRY = 3
	MOD_MOOD_SLEEPY = 4

class Time(Enum):
	MOD_TIME_DAWN = 0
	MOD_TIME_MORNING = 1
	MOD_TIME_MIDDAY = 2
	MOD_TIME_AFTERNOON = 3
	MOD_TIME_DUSK = 4
	MOD_TIME_NIGHT = 5

class Temperature(Enum):
	TEMP_FRIGID = 0
	TEMP_COLD = 1
	TEMP_NEUTRAL = 2
	TEMP_WARM = 3
	TEMP_BOILING = 4