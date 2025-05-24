#include "personality.h"
#include "scenes.h"

uint16_t pet_personality_weights[PET_ATTR_NEG_MAX] = {

	[PET_ATTR_CHARISMA] = 0,
	[PET_ATTR_CONFIDENCE] = 0,
	[PET_ATTR_KINDNESS] = 0,
	[PET_ATTR_PATIENCE] = 0,

	[PET_ATTR_LAZY] = 0,
	[PET_ATTR_RUDE] = 0,
	[PET_ATTR_GASLIGHT] = 0,
	[PET_ATTR_GREEDY] = 0
};

pet_favourites_s pet_favs = {

	.scene = MAIN_SCENE_BEACH,
	.time = MOD_TIME_DUSK,
	.weather = MOD_WEATHER_SUNNY,

	.food = FOOD_KFC,
	.drink = DRINK_BEER,
};
