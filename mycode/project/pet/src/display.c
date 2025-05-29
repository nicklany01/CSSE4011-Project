#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include "mood.h"
#include "display.h"
#include "sound.h"

LOG_MODULE_REGISTER(display, CONFIG_LOG_DEFAULT_LEVEL);

/* Global scene state */
scene_state_s scenes_state = {
    .main_scene = MAIN_SCENE_MEADOW,
    .modifier_mood = MOD_MOOD_NEUTRAL,
    .modifier_weather = MOD_WEATHER_SUNNY,
    .modifier_time = MOD_TIME_DUSK,
    .modifier_temp = TEMP_NEUTRAL};

/* UI Elements */
static lv_obj_t *bars[5];
static lv_obj_t *labels[5];
static lv_obj_t *expression_label;
static lv_obj_t *dot_btns[3];

const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static K_EVENT_DEFINE(display_events);

typedef struct
{
    lv_obj_t *screen;
    lv_obj_t *background;
    lv_obj_t *character;
    lv_obj_t *character_face;
} scene_obj_t;

static scene_obj_t scene_stats;
static scene_obj_t scene_city;
static scene_obj_t scene_forest;

void display_change_scene(main_scene_e scene)
{
    scenes_state.main_scene = scene;

    switch (scene)
    {
    case MAIN_SCENE_MEADOW:
        lv_scr_load(scene_stats.screen);
        scenes_state.current_screen = scene_stats.screen;
        break;
    case MAIN_SCENE_CITY:
        lv_scr_load(scene_city.screen);
        scenes_state.current_screen = scene_city.screen;
        break;
    case MAIN_SCENE_FOREST:
        lv_scr_load(scene_forest.screen);
        scenes_state.current_screen = scene_forest.screen;
        break;
    default:
        break;
    }
}

void scenes_set_main(main_scene_e scene)
{
    display_change_scene(scene);
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

void scenes_draw(void)
{
    // For now just call LVGL's task handler
    lv_task_handler();
}

void display_update_mood(void)
{
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
        LOG_WRN("Mood update skipped (mutex busy)");
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

static void dot_btn_cb(lv_event_t *e)
{
    uint32_t btn_id = (uint32_t)lv_event_get_user_data(e);
    uint32_t event_bit = 0;

    switch (btn_id)
    {
    case 0:
        event_bit = LEFT_DOT_PRESSED;
        break;
    case 1:
        event_bit = CENTER_DOT_PRESSED;
        break;
    case 2:
        event_bit = RIGHT_DOT_PRESSED;
        break;
    }

    if (event_bit)
    {
        display_post_event(event_bit);
        LOG_INF("Dot button %d pressed", btn_id);
    }
}

static void feed_button_cb(lv_event_t *e)
{
    ARG_UNUSED(e);

    if (k_mutex_lock(&mood_mutex, K_MSEC(10)) == 0)
    {
        pet_mood.energy = MIN(pet_mood.energy + 100, MAX_STATE_VALUE);
        k_mutex_unlock(&mood_mutex);

        LOG_INF("Fed pet! Energy: %d", pet_mood.energy);
        sound_play();
        display_update_mood();
    }
    else
    {
        LOG_WRN("Couldn't feed - mood system busy");
    }
}

void create_stats_screen(void)
{
    lv_obj_t *cont = lv_obj_create(scene_stats.screen);
    lv_obj_set_size(cont, LV_PCT(90), LV_PCT(60)); // Reduced height for expression
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // bars and labels
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

    // expression label
    expression_label = lv_label_create(lv_scr_act());
    lv_obj_align(expression_label, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text(expression_label, "NEUTRAL");

    lv_obj_t *btn = lv_btn_create(scene_stats.screen);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(btn, feed_button_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Feed Me!");
    lv_obj_center(btn_label);
}

void create_dot_buttons(void)
{
    static lv_style_t transparent_style;
    lv_style_init(&transparent_style);
    lv_style_set_bg_opa(&transparent_style, LV_OPA_TRANSP);
    lv_style_set_border_opa(&transparent_style, LV_OPA_TRANSP);

    /* Left dot button */
    dot_btns[0] = lv_btn_create(lv_layer_top());
    lv_obj_set_size(dot_btns[0], 40, 40);
    lv_obj_align(dot_btns[0], LV_ALIGN_BOTTOM_LEFT, 10, 10);
    lv_obj_add_style(dot_btns[0], &transparent_style, 0);
    lv_obj_add_event_cb(dot_btns[0], dot_btn_cb, LV_EVENT_CLICKED, (void *)0);

    /* Center dot button */
    dot_btns[1] = lv_btn_create(lv_layer_top());
    lv_obj_set_size(dot_btns[1], 40, 40);
    lv_obj_align(dot_btns[1], LV_ALIGN_BOTTOM_MID, 0, 10);
    lv_obj_add_style(dot_btns[1], &transparent_style, 0);
    lv_obj_add_event_cb(dot_btns[1], dot_btn_cb, LV_EVENT_CLICKED, (void *)1);

    /* Right dot button */
    dot_btns[2] = lv_btn_create(lv_layer_top());
    lv_obj_set_size(dot_btns[2], 40, 40);
    lv_obj_align(dot_btns[2], LV_ALIGN_BOTTOM_RIGHT, -10, 10);
    lv_obj_add_style(dot_btns[2], &transparent_style, 0);
    lv_obj_add_event_cb(dot_btns[2], dot_btn_cb, LV_EVENT_CLICKED, (void *)2);
}

void display_init()
{
    /* Initialize scenes */
    scene_stats.screen = lv_obj_create(NULL);
    scene_city.screen = lv_obj_create(NULL);
    scene_forest.screen = lv_obj_create(NULL);

    /* Create UI elements */
    create_stats_screen();
    create_dot_buttons();

    /* Set initial scene */
    display_change_scene(MAIN_SCENE_MEADOW);
    display_blanking_off(display_dev);
}

void display_post_event(uint32_t events)
{
    k_event_post(&display_events, events);
}

void display_thread(void)
{
    LOG_INF("Display thread started");

    while (1)
    {
        lv_task_handler();

        if (scenes_state.main_scene == MAIN_SCENE_MEADOW)
        {
            display_update_mood();
        }

        /* Wait for any button event */
        uint32_t events = k_event_wait(&display_events,
                                       LEFT_DOT_PRESSED | CENTER_DOT_PRESSED | RIGHT_DOT_PRESSED,
                                       false, K_MSEC(10));

        if (events & LEFT_DOT_PRESSED)
        {
            display_change_scene(MAIN_SCENE_CITY);
        }

        if (events & CENTER_DOT_PRESSED)
        {
            display_change_scene(MAIN_SCENE_MEADOW);
        }

        if (events & RIGHT_DOT_PRESSED)
        {
            display_change_scene(MAIN_SCENE_FOREST);
        }

        k_event_clear(&display_events, events);
        k_sleep(K_MSEC(10));
    }
}

/* Thread definition */
#define DISPLAY_THREAD_STACK_SIZE 2048
#define DISPLAY_THREAD_PRIORITY 5

K_THREAD_STACK_DEFINE(display_thread_stack, DISPLAY_THREAD_STACK_SIZE);
struct k_thread display_thread_data;

void start_display_thread(void)
{
    k_thread_create(&display_thread_data,
                    display_thread_stack,
                    K_THREAD_STACK_SIZEOF(display_thread_stack),
                    (k_thread_entry_t)display_thread,
                    NULL, NULL, NULL,
                    DISPLAY_THREAD_PRIORITY, 0, K_NO_WAIT);
}