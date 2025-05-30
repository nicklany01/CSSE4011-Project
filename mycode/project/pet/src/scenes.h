#ifndef SCENES_H
#define SCENES_H
#include <stdbool.h>
#include <stdint.h>
#include <lvgl.h>
#include "foods.h"
#include "friends.h"

#define MINI_PKT_REG_MAX 20

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

#define MINI_SPRITE_PADDING 10
#define MINI_SPRITE_START 0

#define MINI_SPRITE_MAX 8

#define MINI_PKT_TIMEOUT_TICKS 5000

typedef enum
{
	SPRITE_ZERO,
	SPRITE_ICE,
	SPRITE_CHERRY,
	SPRITE_GRAPE,
	SPRITE_BAJA_BLAST,
	SPRITE_MAX
} sprite_s;

typedef enum
{
	MAIN_SCENE_MEADOW,
	MAIN_SCENE_BEACH,
	MAIN_SCENE_FOREST,
	MAIN_SCENE_CITY,
	MAIN_SCENE_SHOP,
	MAIN_SCENE_MAX
} main_scenes_e;

typedef enum
{
	MOD_WEATHER_SUNNY,
	MOD_WEATHER_RAINY,
	MOD_WEATHER_CLOUDY,
	MOD_WEATHER_SNOWY,
	MOD_WEATHER_MAX
} mod_weather_e;

typedef enum
{
	MOD_MOOD_NEUTRAL,
	MOD_MOOD_HAPPY,
	MOD_MOOD_SAD,
	MOD_MOOD_ANGRY,
	MOD_MOOD_SLEEPY,
	MOD_MOOD_SICK,
	MOD_MOOD_MAX
} mod_mood_e;

typedef enum
{
	MOD_TIME_DAWN,
	MOD_TIME_MORNING,
	MOD_TIME_MIDDAY,
	MOD_TIME_AFTERNOON,
	MOD_TIME_DUSK,
	MOD_TIME_NIGHT,
	MOD_TIME_MAX
} mod_time_e;

typedef enum
{
	TEMP_FRIGID,
	TEMP_COLD,
	TEMP_NEUTRAL,
	TEMP_WARM,
	TEMP_BOILING,
	TEMP_MAX
} mod_temp_e;

typedef struct
{

	pex_uuid_t pex_id;
	sprite_s sprite;
	lv_obj_t *mini;
} mini_register_obj_s;

typedef struct
{

	int64_t last_rx;
	pex_uuid_t pex_id;
	main_scenes_e scene;
	sprite_s sprite;
} mini_timeout_counter_s;

typedef struct
{
	lv_obj_t *base;
	lv_obj_t *face;
	lv_obj_t *anger;
	lv_obj_t *sick;

	mini_register_obj_s mini_register[MINI_SPRITE_MAX];
} character_container_s;

typedef struct
{
	main_scenes_e main_scene;
	mod_weather_e modifier_weather;
	mod_mood_e modifier_mood;
	mod_time_e modifier_time;
	mod_temp_e modifier_temp;
	drinks_e held_drink;
	foods_e held_food;
	bool is_sick;
	bool stats_visible;
	uint32_t colour_sky;
	lv_obj_t *current_screen;
	character_container_s *current_character;
	lv_obj_t *stats_screen;
	sprite_s current_sprite;

	lv_obj_t *wfc_icon;

	bool do_wfc;
	bool allow_wfc;
} scene_state_s;

typedef struct
{
	lv_obj_t *screen;
	lv_obj_t *background;
	character_container_s character;
} scene_obj_meadow_s;

typedef struct
{
	lv_obj_t *screen;
	lv_obj_t *background;
	character_container_s character;
} scene_obj_beach_s;

typedef struct
{
	lv_obj_t *screen;
	lv_obj_t *background;
	character_container_s character;
} scene_obj_forest_s;

typedef struct
{
	lv_obj_t *screen;
	lv_obj_t *background;
	character_container_s character;
} scene_obj_city_s;

typedef struct
{
	lv_obj_t *screen;
	lv_obj_t *background;
	character_container_s character;
} scene_obj_shop_s;

extern scene_state_s scenes_state;
extern mini_timeout_counter_s mini_pkt_register[MINI_PKT_REG_MAX];

void scenes_delete_wfc();
void scenes_allow_wfc();
void scenes_init();
void scenes_draw();
void scenes_set_time(mod_time_e time);
void scenes_set_mood(mod_mood_e mood);
void scenes_set_weather(mod_weather_e weather);
void scenes_set_main(main_scenes_e scene);
void scenes_set_sprite(sprite_s sprite);
bool scenes_process_mini_pkt_rx(int64_t last_rx, main_scenes_e scene, pex_uuid_t pex_id, sprite_s sprite);
void scenes_add_mini(main_scenes_e scene, pex_uuid_t pex_id, sprite_s sprite);
void scenes_adjust_minis(character_container_s *character);
void scenes_remove_mini(main_scenes_e scene, pex_uuid_t pex_id, bool do_reshift);
void scenes_update_mini_pkt_register(int64_t uptime);
void scenes_show_wfc_icon(bool show);
void scenes_toggle_sick();
void scenes_set_temp_from_int_c(int8_t c);
void scenes_create_scene_switch_button(lv_obj_t *parent);
void scenes_create_stats_button(lv_obj_t *parent);
void scenes_toggle_stats();

#endif
