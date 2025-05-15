#ifndef SCENES_H
#define SCENES_H

#include <stdint.h>

#include "lv_demos.h"

#define POSITION_X_CHARACTER 60
#define POSITION_Y_CHARACTER 0

#define POSITION_X_CHAR_FACE (POSITION_X_CHARACTER + 63)
#define POSITION_Y_CHAR_FACE 108

#define POSITION_X_SUN 195
#define POSITION_Y_SUN 10

#define POSITION_X_MEADOW_HILL_BASE 0
#define POSITION_Y_MEADOW_HILL_BASE 150

#define POSITION_X_MEADOW_HILL_TOP 0
#define POSITION_Y_MEADOW_HILL_TOP 0

#define POSITION_Y_FOREST_TREE 75
#define POSITION_Y_FOREST_TREE_BG 45

#define SIZE_W_MEADOW_HILL_BASE 320
#define SIZE_H_MEADOW_HILL_BASE 90

#define COLOUR_MEADOW_HILL_BASE 0x1eac00
#define COLOUR_FOREST_BACKGROUND 0x03250d

#define FOREST_NUM_TREES 5
#define FOREST_NUM_GRASS_ROWS 6
#define FOREST_NUM_GRASS_COLS 20

#define POSITION_Y_FOREST_GRASS POSITION_Y_FOREST_TREE_BG

#define SCREEN_SIZE_W 320
#define SCREEN_SIZE_H 240

typedef enum {

	SPRITE_ZERO,
	SPRITE_CHERRY,
	SPRITE_ICE,
	SPRITE_GRAPE,
	SPRITE_BAJA_BLAST
} sprite_s;

typedef enum {

	MAIN_SCENE_MEADOW,
	MAIN_SCENE_BEACH,
	MAIN_SCENE_FOREST,
	MAIN_SCENE_CITY,
	MAIN_SCENE_SHOP,

	MAIN_SCENE_MAX
} main_scenes_e;

typedef enum {

	MOD_WEATHER_SUNNY,
	MOD_WEATHER_RAINY,
	MOD_WEATHER_CLOUDY,
	MOD_WEATHER_SNOWY,

	MOD_WEATHER_MAX
} mod_weather_e;

typedef enum {

	MOD_MOOD_NEUTRAL,
	MOD_MOOD_HAPPY,
	MOD_MOOD_SAD,
	MOD_MOOD_ANGRY,
	MOD_MOOD_SLEEPY,

	MOD_MOOD_MAX
} mod_mood_e;

typedef enum {

	MOD_TIME_DAWN,
	MOD_TIME_MORNING,
	MOD_TIME_MIDDAY,
	MOD_TIME_AFTERNOON,
	MOD_TIME_DUSK,
	MOD_TIME_NIGHT,

	MOD_TIME_MAX
} mod_time_e;

typedef struct {

	main_scenes_e main_scene;

	mod_weather_e modifier_weather;
	mod_mood_e modifier_mood;
	mod_time_e modifier_time;

	uint32_t colour_sky;
	lv_obj_t *current_screen;

} scene_state_s;

typedef struct {

	lv_obj_t *character;
	lv_obj_t *character_face;

	lv_obj_t *screen;

	lv_obj_t *background;
} scene_obj_meadow_s;

typedef struct {

	lv_obj_t *character;
	lv_obj_t *character_face;

	lv_obj_t *screen;
	lv_obj_t *background;
} scene_obj_beach_s;

typedef struct {

	lv_obj_t *character;
	lv_obj_t *character_face;

	lv_obj_t *background;

	lv_obj_t *screen;

	lv_obj_t *trees[FOREST_NUM_TREES];
	lv_obj_t *trees_bg[FOREST_NUM_TREES];

	lv_obj_t *grass[FOREST_NUM_GRASS_ROWS][FOREST_NUM_GRASS_COLS];
} scene_obj_forest_s;

typedef struct {

	lv_obj_t *character;
	lv_obj_t *character_face;

	lv_obj_t *screen;
} scene_obj_city_s;

typedef struct {

	lv_obj_t *background;

	lv_obj_t *character;
	lv_obj_t *character_face;

	lv_obj_t *screen;
} scene_obj_shop_s;

extern scene_state_s scenes_state;

void scenes_init();
void scenes_draw();

void scenes_set_time(mod_time_e time);
void scenes_set_mood(mod_mood_e mood);
void scenes_set_weather(mod_weather_e weather);
void scenes_set_main(main_scenes_e scene);
#endif
