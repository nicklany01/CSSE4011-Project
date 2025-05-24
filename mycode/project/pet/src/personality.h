#ifndef PERSONALITY_H
#define PERSONALITY_H

#include "scenes.h"
#include "friends.h"

typedef enum {

	FOOD_NONE, // keep me first

	FOOD_PIZZA,
	FOOD_BURGER,
	FOOD_KFC,
	FOOD_SUSHI,
	FOOD_TACOS,
	FOOD_HOT_DOGS,
	FOOD_PASTA,
	FOOD_WINGS,

	FOOD_MAX // keep me last
} foods_e;

typedef enum {

	DRINK_NONE, // keep me first

	DRINK_COLA,
	DRINK_LEMONADE,
	DRINK_MOUNTAIN_DEW,
	DRINK_WATER,
	DRINK_JUICE,
	DRINK_KOMBUCHA,
	DRINK_TEA,
	DRINK_COFFEE,
	DRINK_BEER,

	DRINK_MAX // keep me last
} drinks_e;

typedef enum {

	// 'positive' attributes
	PET_ATTR_CHARISMA,
	PET_ATTR_CONFIDENCE,
	PET_ATTR_KINDNESS,
	PET_ATTR_PATIENCE,

	PET_ATTR_POS_MAX, // keep me before negatives

	// 'negative' attributes
	PET_ATTR_LAZY,
	PET_ATTR_RUDE,
	PET_ATTR_GASLIGHT,
	PET_ATTR_GREEDY,

	PET_ATTR_NEG_MAX // keep me last

} pet_attributes_e;

typedef struct {

	main_scenes_e scene;
	mod_weather_e weather;
	mod_time_e time;

	foods_e food;
	drinks_e drink;

} pet_favourites_s;

extern pex_uuid_t pet_pex_id;
extern uint16_t pet_personality_weights[PET_ATTR_NEG_MAX];

#endif
