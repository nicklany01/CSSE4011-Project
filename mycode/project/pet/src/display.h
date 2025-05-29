#ifndef DISPLAY_H
#define DISPLAY_H

#include <zephyr/kernel.h>
#include <lvgl.h>

/* Scene types to match your existing code */
typedef enum {
    MAIN_SCENE_MEADOW,
    MAIN_SCENE_BEACH,
    MAIN_SCENE_FOREST,
    MAIN_SCENE_CITY,
    MAIN_SCENE_SHOP
} main_scene_e;

typedef enum {
    MOD_MOOD_NEUTRAL,
    MOD_MOOD_ANGRY,
    MOD_MOOD_HAPPY,
    MOD_MOOD_SAD,
    MOD_MOOD_SLEEPY
} mod_mood_e;

typedef enum {
    MOD_WEATHER_SUNNY,
    MOD_WEATHER_CLOUDY,
    MOD_WEATHER_RAINY,
    MOD_WEATHER_SNOWY
} mod_weather_e;

typedef enum {
    MOD_TIME_DAWN,
    MOD_TIME_MORNING,
    MOD_TIME_MIDDAY,
    MOD_TIME_AFTERNOON,
    MOD_TIME_DUSK,
    MOD_TIME_NIGHT
} mod_time_e;

typedef enum {
    TEMP_FRIGID,
    TEMP_COLD,
    TEMP_NEUTRAL,
    TEMP_WARM,
    TEMP_BOILING
} mod_temp_e;

typedef struct {
    main_scene_e main_scene;
    mod_mood_e modifier_mood;
    mod_weather_e modifier_weather;
    mod_time_e modifier_time;
    mod_temp_e modifier_temp;
    uint32_t colour_sky;
    lv_obj_t *current_screen;
    uint8_t held_drink;
    uint8_t held_food;
} scene_state_s;

/* Virtual button event bits */
#define LEFT_DOT_PRESSED   BIT(0)
#define CENTER_DOT_PRESSED BIT(1)
#define RIGHT_DOT_PRESSED  BIT(2)

/* Public API */
void display_init(void);
void display_update_mood(void);
void display_post_event(uint32_t events);
void display_change_scene(main_scene_e scene);
void scenes_set_main(main_scene_e scene);
void scenes_set_mood(mod_mood_e mood);
void scenes_set_weather(mod_weather_e weather);
void scenes_set_time(mod_time_e time);
void scenes_set_temp_from_int_c(int8_t c);
void scenes_draw(void);

/* Global scene state */
extern scene_state_s scenes_state;

#endif /* DISPLAY_H */