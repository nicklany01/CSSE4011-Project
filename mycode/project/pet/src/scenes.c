#include <stddef.h>

#include "scenes.h"
#include "friends.h"
#include "gfx_assets.h"

scene_state_s scenes_state = {

	.main_scene = MAIN_SCENE_MEADOW,
	.modifier_mood = MOD_MOOD_NEUTRAL,
	.modifier_weather = MOD_WEATHER_SUNNY,
	.modifier_time = MOD_TIME_DUSK,

	.current_sprite = SPRITE_ZERO
};

int sprite_face_pos[SPRITE_MAX][2] = {

	[SPRITE_ZERO] = {POSITION_X_CHAR_FACE, POSITION_Y_CHAR_FACE},
	[SPRITE_ICE] = {POSITION_X_CHARACTER + 53, POSITION_Y_CHARACTER + 83},
	[SPRITE_CHERRY] = {POSITION_X_CHARACTER + 47, POSITION_Y_CHARACTER + 86}
};

int anger_icon_pos[SPRITE_MAX][2] = {

	[SPRITE_ZERO] = {POSITION_X_CHARACTER + 121, 49},
	[SPRITE_ICE] = {POSITION_X_CHARACTER + 114, 37},
	[SPRITE_CHERRY] = {POSITION_X_CHARACTER + 122, 35}
};

const lv_image_dsc_t* sprite_0_mood_lookup[MOD_MOOD_MAX] = {
	[MOD_MOOD_NEUTRAL] = &face_neutral,
	[MOD_MOOD_HAPPY] = &face_happy,
	[MOD_MOOD_SAD] = &face_sad,
	[MOD_MOOD_ANGRY] = &face_angry,
	[MOD_MOOD_SLEEPY] = &face_sleepy,
};

const lv_image_dsc_t* sprite_1_mood_lookup[MOD_MOOD_MAX] = {
	[MOD_MOOD_NEUTRAL] = &face_1_happy,
	[MOD_MOOD_HAPPY] = &face_1_happy,
	[MOD_MOOD_SAD] = &face_1_sad,
	[MOD_MOOD_ANGRY] = &face_1_angry,
	[MOD_MOOD_SLEEPY] = &face_1_sleepy,
};

const lv_image_dsc_t* sprite_2_mood_lookup[MOD_MOOD_MAX] = {
	[MOD_MOOD_NEUTRAL] = &face_2_neutral,
	[MOD_MOOD_HAPPY] = &face_2_happy,
	[MOD_MOOD_SAD] = NULL, //&face_2_sad,
	[MOD_MOOD_ANGRY] = &face_2_angry,
	[MOD_MOOD_SLEEPY] = &face_2_sleepy,
};

const lv_image_dsc_t *sprite_base_lookup[SPRITE_MAX] = {

	[SPRITE_ZERO] = &sprite_base,
	[SPRITE_ICE] = &sprite_ice,
	[SPRITE_CHERRY] = &sprite_cherry
};

const lv_image_dsc_t *sprite_sick_lookup[SPRITE_MAX] = {

	[SPRITE_ZERO] = &sprite_sick,
	[SPRITE_ICE] = &sprite_1_sick,
	[SPRITE_CHERRY] = &sprite_2_sick
};

const lv_image_dsc_t *sprite_mini_lookup[SPRITE_MAX] = {

	[SPRITE_ZERO] = &sprite_mini,
	[SPRITE_ICE] = &sprite_1_mini,
	[SPRITE_CHERRY] = &sprite_2_mini
};

scene_obj_meadow_s scene_meadow = {
	.screen = NULL,
};

scene_obj_beach_s scene_beach = {
	.screen = NULL
};

scene_obj_forest_s scene_forest = {
	.screen = NULL
};

scene_obj_city_s scene_city = {
	.screen = NULL
};

scene_obj_shop_s scene_shop = {
	.screen = NULL
};

static uint32_t sky_colours_sunny[] = {

	[MOD_TIME_DAWN] = 0x75f8ff,
	[MOD_TIME_MORNING] = 0x9dfffd,
	[MOD_TIME_MIDDAY] = 0x00c8ff,
	[MOD_TIME_AFTERNOON] = 0xffab58,
	[MOD_TIME_DUSK] = 0x2b4241,
	[MOD_TIME_NIGHT] = 0x000000
};

// 20 is probably more than we'll ever need lmao
mini_timeout_counter_s mini_pkt_register[MINI_PKT_REG_MAX];
int mini_register_idx = 0;

void scenes_set_main(main_scenes_e scene) {
	scenes_state.main_scene = scene;

	switch (scenes_state.main_scene) {
		case MAIN_SCENE_MEADOW:
			scenes_state.current_screen = scene_meadow.screen;
			scenes_state.current_character = &scene_meadow.character;
			break;
		case MAIN_SCENE_BEACH:
			scenes_state.current_screen = scene_beach.screen;
			scenes_state.current_character = &scene_beach.character;
			break;
		case MAIN_SCENE_FOREST:
			scenes_state.current_screen = scene_forest.screen;
			scenes_state.current_character = &scene_forest.character;
			break;
		case MAIN_SCENE_CITY:
			scenes_state.current_screen = scene_city.screen;
			scenes_state.current_character = &scene_city.character;
			break;
		case MAIN_SCENE_SHOP:
			scenes_state.current_screen = scene_shop.screen;
			scenes_state.current_character = &scene_shop.character;
			break;
		default:
			break;
	}

	lv_screen_load(scenes_state.current_screen);
}

void scenes_character_update() {

	const uint8_t *target_sprite = NULL;
	const lv_image_dsc_t *target_face = NULL;
	character_container_s *character = scenes_state.current_character;

	switch (scenes_state.current_sprite) {
		case SPRITE_ZERO:
			target_face = sprite_0_mood_lookup[scenes_state.modifier_mood];
			break;
		case SPRITE_ICE:
			target_face = sprite_1_mood_lookup[scenes_state.modifier_mood];
			break;
		case SPRITE_CHERRY:
			target_face = sprite_2_mood_lookup[scenes_state.modifier_mood];
			break;
		default:
			break;
	}

	lv_image_set_src(character->base, sprite_base_lookup[scenes_state.current_sprite]);

	if (target_face != NULL) {
		lv_image_set_src(character->face, target_face);
		lv_obj_set_pos(character->face, sprite_face_pos[scenes_state.current_sprite][0],
			sprite_face_pos[scenes_state.current_sprite][1]);
	}

	// NOTE: I know it's 'inefficient' to change
	// these flags every draw... and I don't care.

	if (scenes_state.modifier_mood == MOD_MOOD_ANGRY || scenes_state.is_sick) {

		lv_obj_set_pos(character->anger, anger_icon_pos[scenes_state.current_sprite][0],
			anger_icon_pos[scenes_state.current_sprite][1]);
		lv_obj_clear_flag(character->anger, LV_OBJ_FLAG_HIDDEN);
	} else {
		lv_obj_add_flag(character->anger, LV_OBJ_FLAG_HIDDEN);
	}

	if (scenes_state.is_sick) {
		lv_obj_add_flag(character->base, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(character->face, LV_OBJ_FLAG_HIDDEN);

		lv_image_set_src(character->sick, sprite_sick_lookup[scenes_state.current_sprite]);
		lv_obj_clear_flag(character->sick, LV_OBJ_FLAG_HIDDEN);
	} else {
		lv_obj_clear_flag(character->base, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(character->face, LV_OBJ_FLAG_HIDDEN);

		lv_obj_add_flag(character->sick, LV_OBJ_FLAG_HIDDEN);
	}
}

void scenes_forest_init() {

	scene_forest.background = lv_image_create(scene_forest.screen);
	lv_image_set_src(scene_forest.background, &scene_forest_day);
	lv_obj_set_pos(scene_forest.background, 0, 0);
}

void scenes_meadow_init() {

	scene_meadow.background = lv_image_create(scene_meadow.screen);
	lv_image_set_src(scene_meadow.background, &scene_meadow_day);
	lv_obj_set_pos(scene_meadow.background, 0, 0);
}

void scenes_beach_init() {

	scene_beach.background = lv_image_create(scene_beach.screen);
	lv_image_set_src(scene_beach.background, &scene_beach_day);
	lv_obj_set_pos(scene_beach.background, 0, 0);
}

void scenes_shop_init() {

	scene_shop.background = lv_image_create(scene_shop.screen);
	lv_image_set_src(scene_shop.background, &scene_shop_day);
	lv_obj_set_pos(scene_shop.background, 0, 0);
}

void scenes_city_init() {

	scene_shop.background = lv_image_create(scene_shop.screen);
	//lv_image_set_src(scene_shop.background, &scene_shop_day);
	lv_obj_set_pos(scene_shop.background, 0, 0);
}

void scenes_forest_update() {

	switch (scenes_state.modifier_time) {
		case MOD_TIME_DAWN:
		case MOD_TIME_MORNING:
		case MOD_TIME_MIDDAY:
		case MOD_TIME_AFTERNOON:

			lv_image_set_src(scene_forest.background, &scene_forest_day);
			break;
		case MOD_TIME_DUSK:
		case MOD_TIME_NIGHT:

			lv_image_set_src(scene_forest.background, &scene_forest_night);
			break;
		default:
			return;
	}

}

void scenes_meadow_update() {

	switch (scenes_state.modifier_time) {
		case MOD_TIME_DAWN:
		case MOD_TIME_MORNING:
		case MOD_TIME_MIDDAY:
		case MOD_TIME_AFTERNOON:
			lv_image_set_src(scene_meadow.background, &scene_meadow_day);
			break;
		case MOD_TIME_DUSK:
		case MOD_TIME_NIGHT:
			lv_image_set_src(scene_meadow.background, &scene_meadow_night);
			break;
		default:
			return;
	}
}

void scenes_beach_update() {

	switch (scenes_state.modifier_time) {
		case MOD_TIME_DAWN:
		case MOD_TIME_MORNING:
		case MOD_TIME_MIDDAY:
		case MOD_TIME_AFTERNOON:

			lv_image_set_src(scene_beach.background, &scene_beach_day);
			break;
		case MOD_TIME_DUSK:
		case MOD_TIME_NIGHT:

			lv_image_set_src(scene_beach.background, &scene_beach_night);
			break;
		default:
			return;
	}

}

void scenes_city_update() {
	return;
}

void scenes_set_mood(mod_mood_e mood) {
	scenes_state.modifier_mood = mood;
}

void scenes_set_weather(mod_weather_e weather) {
	scenes_state.modifier_weather = weather;
}

void scenes_set_time(mod_time_e time) {
	scenes_state.modifier_time = time;
}

void scenes_set_sprite(sprite_s sprite) {
	scenes_state.current_sprite = sprite;
	scenes_character_update();
}

void scenes_set_is_sick(bool sick) {
	scenes_state.is_sick = sick;
}

void scenes_toggle_sick() {
	scenes_state.is_sick = !scenes_state.is_sick;
}

void scenes_set_temp_from_int_c(int8_t c) {

	if (c < 0) {
		scenes_state.modifier_temp = TEMP_FRIGID;
	} else if (c < 18) {
		scenes_state.modifier_temp = TEMP_COLD;
	} else if (c < 25) {
		scenes_state.modifier_temp = TEMP_NEUTRAL;
	} else if (c < 35) {
		scenes_state.modifier_temp = TEMP_WARM;
	} else {
		scenes_state.modifier_temp = TEMP_BOILING;
	}
}

void scenes_init_character(main_scenes_e scene) {

	character_container_s *character;
	lv_obj_t *screen;

	switch (scene) {
		case MAIN_SCENE_MEADOW:
			character = &scene_meadow.character;
			screen = scene_meadow.screen;
			break;

		case MAIN_SCENE_BEACH:
			character = &scene_beach.character;
			screen = scene_beach.screen;
			break;
		case MAIN_SCENE_CITY:
			character = &scene_city.character;
			screen = scene_city.screen;
			break;
		case MAIN_SCENE_FOREST:
			character = &scene_forest.character;
			screen = scene_forest.screen;
			break;
		case MAIN_SCENE_SHOP:
			character = &scene_shop.character;
			screen = scene_shop.screen;
			break;
		default:
			return;
	}

	character->sick = lv_image_create(screen);
	character->base = lv_image_create(screen);
	character->face = lv_image_create(screen);
	character->anger = lv_image_create(screen);

	lv_image_set_src(character->anger, &anger);
	lv_obj_set_pos(character->base, POSITION_X_CHARACTER, POSITION_Y_CHARACTER);
	lv_obj_set_pos(character->sick, POSITION_X_CHARACTER, POSITION_Y_CHARACTER);

	for (int i = 0; i < MINI_SPRITE_MAX; i++) {
		character->mini_register[i].sprite = -1;
		character->mini_register[i].pex_id = 0;

		character->mini_register[i].mini = lv_image_create(screen);
		lv_obj_set_pos(character->mini_register[i].mini, 26*i + (MINI_SPRITE_PADDING * (i + 1)), 5);
		lv_obj_add_flag(character->mini_register[i].mini, LV_OBJ_FLAG_HIDDEN);
	}
	scenes_state.current_character = character;
	scenes_character_update();
}

void scenes_adjust_minis(character_container_s *character) {
	mini_register_obj_s *obj;
	mini_register_obj_s *scratch_obj;
	mini_register_obj_s scratch_register[MINI_SPRITE_MAX];
	int runner = 0;

	// i know in place would be better but ceebs at this point tbh

	for (int i = 0; i < MINI_SPRITE_MAX; i++) {
		obj = &character->mini_register[i];
		if (obj->pex_id == 0) {
			// this condition means
			// a mini is now invalid
			scratch_register[i].pex_id = 0;
			scratch_register[i].sprite = -1;
			continue;
		}

		scratch_register[runner].pex_id = obj->pex_id;
		scratch_register[runner].sprite = obj->sprite;
		runner++;
	}

	for (int i = 0; i < MINI_SPRITE_MAX; i++) {
		obj = &character->mini_register[i];
		scratch_obj = &scratch_register[i];

		obj->pex_id = scratch_obj->pex_id;
		obj->sprite = scratch_obj->sprite;

		if (obj->pex_id != 0 && obj->sprite != -1) {
			lv_image_set_src(obj->mini, sprite_mini_lookup[obj->sprite]);
			lv_obj_clear_flag(obj->mini, LV_OBJ_FLAG_HIDDEN);
		} else {
			lv_obj_add_flag(obj->mini, LV_OBJ_FLAG_HIDDEN);
		}
	}
}

void scenes_remove_mini(main_scenes_e scene, pex_uuid_t pex_id, bool do_reshift) {

	character_container_s *character;
	mini_register_obj_s *obj;

	switch (scene) {
		case MAIN_SCENE_MEADOW:
			character = &scene_meadow.character;
			break;
		case MAIN_SCENE_BEACH:
			character = &scene_beach.character;
			break;
		case MAIN_SCENE_CITY:
			character = &scene_city.character;
			break;
		case MAIN_SCENE_FOREST:
			character = &scene_forest.character;
			break;
		case MAIN_SCENE_SHOP:
			character = &scene_shop.character;
			break;
		default:
			return;
	}

	for (int i = 0; i < MINI_SPRITE_MAX; i++) {
		obj = &character->mini_register[i];
		if (obj->pex_id == pex_id) {
			obj->pex_id = 0;
			break;
		}
	}

	if (do_reshift) {
		// reshift the register
		scenes_adjust_minis(character);
	}
}

void scenes_update_mini_pkt_register(int64_t uptime) {
	mini_timeout_counter_s *counter;
	for (int i = 0; i < MINI_PKT_REG_MAX; i++) {
		counter = &mini_pkt_register[i];
		if (uptime - counter->last_rx > MINI_PKT_TIMEOUT_TICKS) {
			scenes_remove_mini(counter->scene, counter->pex_id, true);

			counter->last_rx = 0;
			counter->pex_id = 0;
			counter->scene = 0;
		}
	}
}

void scenes_add_mini(main_scenes_e scene, pex_uuid_t pex_id, sprite_s sprite) {

	// first check if we're already there
	character_container_s *character;
	mini_register_obj_s *obj;

	switch (scene) {
		case MAIN_SCENE_MEADOW:
			character = &scene_meadow.character;
			break;
		case MAIN_SCENE_BEACH:
			character = &scene_beach.character;
			break;
		case MAIN_SCENE_CITY:
			character = &scene_city.character;
			break;
		case MAIN_SCENE_FOREST:
			character = &scene_forest.character;
			break;
		case MAIN_SCENE_SHOP:
			character = &scene_shop.character;
			break;
		default:
			return;
	}

	int next_free = -1;

	for (int i = 0; i < MINI_SPRITE_MAX; i++) {
		obj = &character->mini_register[i];
		if (obj->pex_id == pex_id) {
			if (obj->sprite == sprite) {
				// dnc
				return;
			}
			// sprite changed
			lv_image_set_src(obj->mini, sprite_mini_lookup[sprite]);
			return;
		}

		if (obj->pex_id == 0 && next_free == -1) {
			next_free = i;
		}
	}

	// set up mini
	obj = &character->mini_register[next_free];
	obj->pex_id = pex_id;
	obj->sprite = sprite;

	lv_image_set_src(obj->mini, sprite_mini_lookup[sprite]);
	lv_obj_clear_flag(obj->mini, LV_OBJ_FLAG_HIDDEN);
}

void scenes_process_mini_pkt_rx(int64_t last_rx, main_scenes_e scene, pex_uuid_t pex_id, sprite_s sprite) {

	mini_timeout_counter_s *counter;
	int next_free = -1;

	for (int i = 0; i < MINI_PKT_REG_MAX; i++) {
		counter = &mini_pkt_register[i];
		if (counter->pex_id == pex_id) {
			counter->pex_id = pex_id;

			if (counter->scene != scene) {
				// we had a scene change, get rid of it
				scenes_remove_mini(counter->scene, pex_id, true);
			} else if (counter->sprite != sprite) {
				// we had a change of sprite, change it
				scenes_add_mini(counter->scene, counter->pex_id, counter->sprite);
			}

			counter->scene = scene;
			counter->sprite = sprite;
			counter->last_rx = last_rx;

			return;
		}

		if (next_free == -1 && counter->pex_id == 0) {
			next_free = i;
		}
	}

	// if we are here we are brand new
	counter = &mini_pkt_register[next_free];

	counter->pex_id = pex_id;
	counter->scene = scene;
	counter->sprite = sprite;
	counter->last_rx = last_rx;

	scenes_add_mini(counter->scene, counter->pex_id, counter->sprite);
}

void scenes_init() {

	for (int i = 0; i < MINI_PKT_REG_MAX; i++) {
		mini_pkt_register[i].last_rx = 0;
		mini_pkt_register[i].pex_id = 0;
		mini_pkt_register[i].scene = 0;
		mini_pkt_register[i].sprite = 0;
	}

	scene_meadow.screen = lv_obj_create(NULL);
	scene_beach.screen = lv_obj_create(NULL);
	scene_forest.screen = lv_obj_create(NULL);
	scene_city.screen = lv_obj_create(NULL);
	scene_shop.screen = lv_obj_create(NULL);

	scenes_meadow_init();
	scenes_forest_init();
	scenes_beach_init();
	scenes_city_init();
	scenes_shop_init();

	scenes_init_character(MAIN_SCENE_MEADOW);
	scenes_init_character(MAIN_SCENE_FOREST);
	scenes_init_character(MAIN_SCENE_BEACH);
	scenes_init_character(MAIN_SCENE_CITY);
	scenes_init_character(MAIN_SCENE_SHOP);

	scenes_set_main(MAIN_SCENE_MEADOW);
}

void scenes_setup_weather() {

	switch (scenes_state.modifier_weather) {
		case MOD_WEATHER_SUNNY:
			scenes_state.colour_sky = sky_colours_sunny[scenes_state.modifier_time];
			break;
		case MOD_WEATHER_CLOUDY:
			break;
		case MOD_WEATHER_RAINY:
			break;
		case MOD_WEATHER_SNOWY:
			break;
		default:
			break;
	}

	lv_obj_set_style_bg_color(lv_screen_active(),
		lv_color_hex(scenes_state.colour_sky), LV_PART_MAIN);
}

void screens_city_update() {

}

void scenes_draw() {

	switch (scenes_state.main_scene) {
		case MAIN_SCENE_MEADOW:
			scenes_meadow_update();
			break;
		case MAIN_SCENE_BEACH:
			scenes_beach_update();
			break;
		case MAIN_SCENE_CITY:
			screens_city_update();
			break;
		case MAIN_SCENE_FOREST:
			scenes_forest_update();
			break;
		case MAIN_SCENE_SHOP:
			break;
		default:
			break;
	}

	scenes_character_update();
	lv_refr_now(NULL);
	lv_task_handler();
}
