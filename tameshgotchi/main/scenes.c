#include "scenes.h"
#include "gfx_assets.h"

scene_state_s scenes_state = {

	.main_scene = MAIN_SCENE_MEADOW,
	.modifier_mood = MOD_MOOD_NEUTRAL,
	.modifier_weather = MOD_WEATHER_SUNNY,
	.modifier_time = MOD_TIME_DUSK
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

void scenes_set_main(main_scenes_e scene) {
	scenes_state.main_scene = scene;

	switch (scenes_state.main_scene) {
		case MAIN_SCENE_MEADOW:
			scenes_state.current_screen = scene_meadow.screen;
			break;
		case MAIN_SCENE_BEACH:
			scenes_state.current_screen = scene_beach.screen;
			break;
		case MAIN_SCENE_FOREST:
			scenes_state.current_screen = scene_forest.screen;
			break;
		case MAIN_SCENE_CITY:
			scenes_state.current_screen = scene_city.screen;
			break;
		case MAIN_SCENE_SHOP:
			scenes_state.current_screen = scene_shop.screen;
			break;
		default:
			break;
	}

	lv_screen_load(scenes_state.current_screen);
}

void scenes_character_update() {

	lv_obj_t *character_face;

	switch (scenes_state.main_scene) {
		case MAIN_SCENE_MEADOW:
			character_face = scene_meadow.character_face;
			break;
		case MAIN_SCENE_BEACH:
			character_face = scene_beach.character_face;
			break;
		case MAIN_SCENE_CITY:
			character_face = scene_city.character_face;
			break;
		case MAIN_SCENE_FOREST:
			character_face = scene_forest.character_face;
			break;
		case MAIN_SCENE_SHOP:
			character_face = scene_shop.character_face;
			break;
		default:
			return;
	}

	switch (scenes_state.modifier_mood) {
		case MOD_MOOD_NEUTRAL:
		case MOD_MOOD_ANGRY:
			lv_image_set_src(character_face, &face_angry);
			break;
		case MOD_MOOD_HAPPY:
			lv_image_set_src(character_face, &face_happy);
			break;
		case MOD_MOOD_SAD:
			lv_image_set_src(character_face, &face_sad);
			break;
		case MOD_MOOD_SLEEPY:
			lv_image_set_src(character_face, &face_sleepy);
			break;
		default:
			break;
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

void scenes_set_mood(mod_mood_e mood) {
	scenes_state.modifier_mood = mood;
}

void scenes_set_weather(mod_weather_e weather) {
	scenes_state.modifier_weather = weather;
}

void scenes_set_time(mod_time_e time) {
	scenes_state.modifier_time = time;
}

void scenes_init_character(main_scenes_e scene) {

	lv_obj_t *character;
	lv_obj_t *character_face;
	lv_obj_t *screen;

	switch (scene) {
		case MAIN_SCENE_MEADOW:
			scene_meadow.character = lv_image_create(scene_meadow.screen);
			scene_meadow.character_face = lv_image_create(scene_meadow.screen);

			character = scene_meadow.character;
			character_face = scene_meadow.character_face;
			break;

		case MAIN_SCENE_BEACH:
			scene_beach.character = lv_image_create(scene_beach.screen);
			scene_beach.character_face = lv_image_create(scene_beach.screen);

			character = scene_beach.character;
			character_face = scene_beach.character_face;
			break;
		case MAIN_SCENE_CITY:
			scene_city.character = lv_image_create(scene_city.screen);
			scene_city.character_face = lv_image_create(scene_city.screen);

			character = scene_city.character;
			character_face = scene_city.character_face;
			break;
		case MAIN_SCENE_FOREST:
			scene_forest.character = lv_image_create(scene_forest.screen);
			scene_forest.character_face = lv_image_create(scene_forest.screen);

			character = scene_forest.character;
			character_face = scene_forest.character_face;
			break;
		case MAIN_SCENE_SHOP:
			scene_shop.character = lv_image_create(scene_shop.screen);
			scene_shop.character_face = lv_image_create(scene_shop.screen);

			character = scene_shop.character;
			character_face = scene_shop.character_face;
			break;
		default:
			return;
	}

	lv_image_set_src(character, &sprite_base);
	lv_obj_set_pos(character, POSITION_X_CHARACTER, POSITION_Y_CHARACTER);

	lv_image_set_src(character_face, &face_happy);
	lv_obj_set_pos(character_face, POSITION_X_CHAR_FACE, POSITION_Y_CHAR_FACE);
}

void scenes_init() {

	scene_meadow.screen = lv_obj_create(NULL);
	scene_beach.screen = lv_obj_create(NULL);
	scene_forest.screen = lv_obj_create(NULL);
	scene_city.screen = lv_obj_create(NULL);
	scene_shop.screen = lv_obj_create(NULL);

	scenes_meadow_init();
	scenes_forest_init();
	scenes_beach_init();
	scenes_shop_init();

	scenes_init_character(MAIN_SCENE_MEADOW);
	scenes_init_character(MAIN_SCENE_FOREST);
	scenes_init_character(MAIN_SCENE_BEACH);
	scenes_init_character(MAIN_SCENE_SHOP);

	scenes_set_main(MAIN_SCENE_FOREST);
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

void scenes_draw() {

	switch (scenes_state.main_scene) {
		case MAIN_SCENE_MEADOW:
			scenes_meadow_update();
			break;
		case MAIN_SCENE_BEACH:
			scenes_beach_update();
			break;
		case MAIN_SCENE_CITY:
			lv_screen_load(scene_city.screen);
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
}
