#include <stddef.h>
#include "scenes.h"
#include "gfx_assets.h"
#include "mood.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(scenes, CONFIG_LOG_DEFAULT_LEVEL);

scene_state_s scenes_state = {
	.main_scene = MAIN_SCENE_MEADOW,
	.modifier_mood = MOD_MOOD_NEUTRAL,
	.modifier_weather = MOD_WEATHER_SUNNY,
	.modifier_time = MOD_TIME_DUSK,
	.current_sprite = SPRITE_ZERO,
	.stats_visible = false,
	.stats_screen = NULL};

int sprite_face_pos[SPRITE_MAX][2] = {
	[SPRITE_ZERO] = {POSITION_X_CHAR_FACE, POSITION_Y_CHAR_FACE},
	[SPRITE_ICE] = {POSITION_X_CHARACTER + 53, POSITION_Y_CHARACTER + 83},
	[SPRITE_CHERRY] = {POSITION_X_CHARACTER + 47, POSITION_Y_CHARACTER + 86}};

int anger_icon_pos[SPRITE_MAX][2] = {
	[SPRITE_ZERO] = {POSITION_X_CHARACTER + 121, 49},
	[SPRITE_ICE] = {POSITION_X_CHARACTER + 114, 37},
	[SPRITE_CHERRY] = {POSITION_X_CHARACTER + 122, 35}};

const lv_image_dsc_t *sprite_0_mood_lookup[MOD_MOOD_MAX] = {
	[MOD_MOOD_NEUTRAL] = &face_neutral,
	[MOD_MOOD_HAPPY] = &face_happy,
	[MOD_MOOD_SAD] = &face_sad,
	[MOD_MOOD_ANGRY] = &face_angry,
	[MOD_MOOD_SLEEPY] = &face_sleepy,
};

const lv_image_dsc_t *sprite_1_mood_lookup[MOD_MOOD_MAX] = {
	[MOD_MOOD_NEUTRAL] = &face_1_happy,
	[MOD_MOOD_HAPPY] = &face_1_happy,
	[MOD_MOOD_SAD] = &face_1_sad,
	[MOD_MOOD_ANGRY] = &face_1_angry,
	[MOD_MOOD_SLEEPY] = &face_1_sleepy,
};

const lv_image_dsc_t *sprite_2_mood_lookup[MOD_MOOD_MAX] = {
	[MOD_MOOD_NEUTRAL] = &face_2_neutral,
	[MOD_MOOD_HAPPY] = &face_2_happy,
	[MOD_MOOD_SAD] = NULL, //&face_2_sad,
	[MOD_MOOD_ANGRY] = &face_2_angry,
	[MOD_MOOD_SLEEPY] = &face_2_sleepy,
};

const lv_image_dsc_t *sprite_base_lookup[SPRITE_MAX] = {
	[SPRITE_ZERO] = &sprite_base,
	[SPRITE_ICE] = &sprite_ice,
	[SPRITE_CHERRY] = &sprite_cherry};

const lv_image_dsc_t *sprite_sick_lookup[SPRITE_MAX] = {
	[SPRITE_ZERO] = &sprite_sick,
	[SPRITE_ICE] = &sprite_1_sick,
	[SPRITE_CHERRY] = &sprite_2_sick};

scene_obj_meadow_s scene_meadow = {
	.screen = NULL,
};

scene_obj_beach_s scene_beach = {
	.screen = NULL};

scene_obj_forest_s scene_forest = {
	.screen = NULL};

scene_obj_city_s scene_city = {
	.screen = NULL};

scene_obj_shop_s scene_shop = {
	.screen = NULL};

static uint32_t sky_colours_sunny[] = {
	[MOD_TIME_DAWN] = 0x75f8ff,
	[MOD_TIME_MORNING] = 0x9dfffd,
	[MOD_TIME_MIDDAY] = 0x00c8ff,
	[MOD_TIME_AFTERNOON] = 0xffab58,
	[MOD_TIME_DUSK] = 0x2b4241,
	[MOD_TIME_NIGHT] = 0x000000};

static void scene_switch_button_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED)
	{
		// Cycle through scenes
		main_scenes_e next_scene = (scenes_state.main_scene + 1) % (MAIN_SCENE_MAX);
		scenes_set_main(next_scene);
	}
}

static void stats_button_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED)
	{
		scenes_toggle_stats();
	}
}

void scenes_create_scene_switch_button(lv_obj_t *parent)
{
	lv_obj_t *btn = lv_button_create(parent);
	lv_obj_set_size(btn, 100, 50);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
	lv_obj_t *label = lv_label_create(btn);
	lv_label_set_text(label, "Next Scene");
	lv_obj_center(label);
	lv_obj_add_event_cb(btn, scene_switch_button_cb, LV_EVENT_CLICKED, NULL);
}

void scenes_create_stats_button(lv_obj_t *parent)
{
	lv_obj_t *btn = lv_button_create(parent);
	lv_obj_set_size(btn, 80, 40);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
	lv_obj_t *label = lv_label_create(btn);
	lv_label_set_text(label, "Stats");
	lv_obj_center(label);
	lv_obj_add_event_cb(btn, stats_button_cb, LV_EVENT_CLICKED, NULL);
}

static lv_obj_t *bars[5];
static lv_obj_t *labels[5];
static lv_obj_t *expression_label;

void scenes_create_stats_screen()
{
	if (scenes_state.stats_screen == NULL)
	{
		scenes_state.stats_screen = lv_obj_create(NULL);

		// Create container for mood bars
		lv_obj_t *cont = lv_obj_create(scenes_state.stats_screen);
		lv_obj_set_size(cont, LV_PCT(90), LV_PCT(60));
		lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 20);
		lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		// Create bars and labels for each stat
		const char *names[] = {"Affection", "Happiness", "Energy", "Health", "Interaction"};
		for (int i = 0; i < 5; i++)
		{
			lv_obj_t *bar_cont = lv_obj_create(cont);
			lv_obj_set_size(bar_cont, LV_PCT(18), LV_PCT(100));
			lv_obj_set_style_pad_all(bar_cont, 5, 0);

			bars[i] = lv_bar_create(bar_cont);
			lv_obj_set_size(bars[i], LV_PCT(100), LV_PCT(70));
			lv_bar_set_range(bars[i], 0, 100);
			lv_obj_align(bars[i], LV_ALIGN_TOP_MID, 0, 0);

			labels[i] = lv_label_create(bar_cont);
			lv_label_set_text(labels[i], names[i]);
			lv_obj_align(labels[i], LV_ALIGN_BOTTOM_MID, 0, 0);
		}

		// Create expression label (current mood)
		expression_label = lv_label_create(scenes_state.stats_screen);
		lv_obj_align(expression_label, LV_ALIGN_TOP_MID, 0, 0);

		// Update the mood display initially
		scenes_update_stats_display();

		// Add stats button to go back
		scenes_create_stats_button(scenes_state.stats_screen);
	}
}

void scenes_update_stats_display()
{
	if (scenes_state.stats_screen == NULL)
		return;

	const char *names[] = {"Affection", "Happiness", "Energy", "Health", "Interaction"};
	struct mood_state local_copy;

	// Safely copy mood state
	if (k_mutex_lock(&mood_mutex, K_MSEC(10)) == 0)
	{
		memcpy(&local_copy, &pet_mood, sizeof(local_copy));
		k_mutex_unlock(&mood_mutex);
	}
	else
	{
		return;
	}

	for (int i = 0; i < 5; i++)
	{
		int value;
		switch (i)
		{
		case 0:
			value = local_copy.affection;
			break;
		case 1:
			value = local_copy.happiness;
			break;
		case 2:
			value = local_copy.energy;
			break;
		case 3:
			value = local_copy.health;
			break;
		case 4:
			value = local_copy.interaction;
			break;
		default:
			value = 0;
			break;
		}

		lv_bar_set_value(bars[i], value / 10, LV_ANIM_ON);
		char buf[32];
		snprintf(buf, sizeof(buf), "%s: %d", names[i], value);
		lv_label_set_text(labels[i], buf);
	}

	if (local_copy.expression >= EXPRESSION_ENLIGHTENED &&
		local_copy.expression <= EXPRESSION_SLEEPY)
	{
		lv_label_set_text(expression_label,
						  expression_strings[local_copy.expression]);
	}
	else
	{
		lv_label_set_text(expression_label, "UNKNOWN");
	}
}
void scenes_toggle_stats()
{
	scenes_state.stats_visible = !scenes_state.stats_visible;

	if (scenes_state.stats_visible)
	{
		scenes_create_stats_screen();
		lv_screen_load(scenes_state.stats_screen);
	}
	else
	{
		lv_screen_load(scenes_state.current_screen);
	}
}

void scenes_set_main(main_scenes_e scene)
{
	scenes_state.main_scene = scene;
	switch (scenes_state.main_scene)
	{
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

	// Only load the scene screen if stats is not visible
	if (!scenes_state.stats_visible)
	{
		lv_screen_load(scenes_state.current_screen);
	}
}

void scenes_character_update()
{
	const uint8_t *target_sprite = NULL;
	const lv_image_dsc_t *target_face = NULL;
	character_container_s *character = NULL;

	switch (scenes_state.main_scene)
	{
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

	switch (scenes_state.current_sprite)
	{
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
	if (target_face != NULL)
	{
		lv_image_set_src(character->face, target_face);
		lv_obj_set_pos(character->face, sprite_face_pos[scenes_state.current_sprite][0],
					   sprite_face_pos[scenes_state.current_sprite][1]);
	}

	// NOTE: I know it's 'inefficient' to change
	// these flags every draw... and I don't care.
	if (scenes_state.modifier_mood == MOD_MOOD_ANGRY || scenes_state.is_sick)
	{
		lv_obj_set_pos(character->anger, anger_icon_pos[scenes_state.current_sprite][0],
					   anger_icon_pos[scenes_state.current_sprite][1]);
		lv_obj_clear_flag(character->anger, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(character->anger, LV_OBJ_FLAG_HIDDEN);
	}

	if (scenes_state.is_sick)
	{
		lv_obj_add_flag(character->base, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(character->face, LV_OBJ_FLAG_HIDDEN);
		lv_image_set_src(character->sick, sprite_sick_lookup[scenes_state.current_sprite]);
		lv_obj_clear_flag(character->sick, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_clear_flag(character->base, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(character->face, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(character->sick, LV_OBJ_FLAG_HIDDEN);
	}
}

void scenes_forest_init()
{
	scene_forest.background = lv_image_create(scene_forest.screen);
	lv_image_set_src(scene_forest.background, &scene_forest_day);
	lv_obj_set_pos(scene_forest.background, 0, 0);
	scenes_create_scene_switch_button(scene_forest.screen);
	scenes_create_stats_button(scene_forest.screen);
}

void scenes_meadow_init()
{
	scene_meadow.background = lv_image_create(scene_meadow.screen);
	lv_image_set_src(scene_meadow.background, &scene_meadow_day);
	lv_obj_set_pos(scene_meadow.background, 0, 0);
	scenes_create_scene_switch_button(scene_meadow.screen);
	scenes_create_stats_button(scene_meadow.screen);
}

void scenes_beach_init()
{
	scene_beach.background = lv_image_create(scene_beach.screen);
	lv_image_set_src(scene_beach.background, &scene_beach_day);
	lv_obj_set_pos(scene_beach.background, 0, 0);
	scenes_create_scene_switch_button(scene_beach.screen);
	scenes_create_stats_button(scene_beach.screen);
}

void scenes_shop_init()
{
	scene_shop.background = lv_image_create(scene_shop.screen);
	lv_image_set_src(scene_shop.background, &scene_shop_day);
	lv_obj_set_pos(scene_shop.background, 0, 0);
	scenes_create_scene_switch_button(scene_shop.screen);
	scenes_create_stats_button(scene_shop.screen);
}

void scenes_city_init()
{
	scene_city.background = lv_image_create(scene_city.screen);
	// lv_image_set_src(scene_city.background, &scene_city_day);
	lv_obj_set_pos(scene_city.background, 0, 0);
	scenes_create_scene_switch_button(scene_city.screen);
	scenes_create_stats_button(scene_city.screen);
}

void scenes_forest_update()
{
	switch (scenes_state.modifier_time)
	{
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

void scenes_meadow_update()
{
	switch (scenes_state.modifier_time)
	{
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

void scenes_beach_update()
{
	switch (scenes_state.modifier_time)
	{
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

void scenes_city_update()
{
	return;
}

void scenes_set_mood(mod_mood_e mood)
{
	scenes_state.modifier_mood = mood;
}

void scenes_set_weather(mod_weather_e weather)
{
	scenes_state.modifier_weather = weather;
}

void scenes_set_time(mod_time_e time)
{
	scenes_state.modifier_time = time;
}

void scenes_set_sprite(sprite_s sprite)
{
	scenes_state.current_sprite = sprite;
	scenes_character_update();
}

void scenes_set_is_sick(bool sick)
{
	scenes_state.is_sick = sick;
}

void scenes_toggle_sick()
{
	scenes_state.is_sick = !scenes_state.is_sick;
}

void scenes_set_temp_from_int_c(int8_t c)
{
	if (c < 0)
	{
		scenes_state.modifier_temp = TEMP_FRIGID;
	}
	else if (c < 18)
	{
		scenes_state.modifier_temp = TEMP_COLD;
	}
	else if (c < 25)
	{
		scenes_state.modifier_temp = TEMP_NEUTRAL;
	}
	else if (c < 35)
	{
		scenes_state.modifier_temp = TEMP_WARM;
	}
	else
	{
		scenes_state.modifier_temp = TEMP_BOILING;
	}
}

void scenes_init_character(main_scenes_e scene)
{
	character_container_s *character;
	lv_obj_t *screen;
	switch (scene)
	{
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
	scenes_character_update();
}

void scenes_init()
{
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

	scenes_set_main(MAIN_SCENE_FOREST);
}

void scenes_setup_weather()
{
	switch (scenes_state.modifier_weather)
	{
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

void screens_city_update()
{
}

void scenes_draw()
{
	// Only update scene content if stats screen is not visible
	if (!scenes_state.stats_visible)
	{
		switch (scenes_state.main_scene)
		{
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
	}
	else
	{
		scenes_update_stats_display();
	}

	lv_refr_now(NULL);
	lv_task_handler();
}