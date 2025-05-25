#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include "mood.h"
#include "display.h"
#include "sound.h"

LOG_MODULE_REGISTER(display, CONFIG_LOG_DEFAULT_LEVEL);

static lv_obj_t *bars[5];
static lv_obj_t *labels[5];

void display_update_mood(void)
{
    const char *names[] = {"Affection", "Happiness", "Energy", "Health", "Interaction"};
    struct mood_state local_copy;
    
    if(k_mutex_lock(&mood_mutex, K_MSEC(100)) == 0) {
        memcpy(&local_copy, &pet_mood, sizeof(local_copy));
        k_mutex_unlock(&mood_mutex);
    } else {
        LOG_ERR("Failed to lock mood mutex");
        return;
    }

    for (int i = 0; i < 5; i++) {
        int value;
        switch (i) {
            case 0: value = local_copy.affection; break;
            case 1: value = local_copy.happiness; break;
            case 2: value = local_copy.energy; break;
            case 3: value = local_copy.health; break;
            case 4: value = local_copy.interaction; break;
            default: value = 0; break;
        }

        lv_bar_set_value(bars[i], value / 10, LV_ANIM_ON);
        char buf[32];
        snprintf(buf, sizeof(buf), "%s: %d", names[i], value);
        lv_label_set_text(labels[i], buf);
    }
}
static void feed_button_cb(lv_event_t *e)
{
    ARG_UNUSED(e);
    LOG_INF("Nice");
    pet_mood.energy = MIN(pet_mood.energy + 100, MAX_STATE_VALUE);
    display_update_mood();
    sound_play();
}

void display_init(const struct device *display_dev)
{
    // Create container for mood bars
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, LV_PCT(90), LV_PCT(70));
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Create bars and labels for each mood state
    const char *names[] = {"Affection", "Happiness", "Energy", "Health", "Interaction"};
    
    for (int i = 0; i < 5; i++) {
        // Create container for each bar+label pair
        lv_obj_t *bar_cont = lv_obj_create(cont);
        lv_obj_set_size(bar_cont, LV_PCT(18), LV_PCT(100));
        lv_obj_set_style_pad_all(bar_cont, 5, 0);
        
        // Create bar
        bars[i] = lv_bar_create(bar_cont);
        lv_obj_set_size(bars[i], LV_PCT(100), LV_PCT(70));
        lv_bar_set_range(bars[i], 0, 100); // 0-100% scale
        lv_obj_align(bars[i], LV_ALIGN_TOP_MID, 0, 0);
        
        // Create label
        labels[i] = lv_label_create(bar_cont);
        lv_label_set_text(labels[i], names[i]);
        lv_obj_align(labels[i], LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_set_style_text_align(labels[i], LV_TEXT_ALIGN_CENTER, 0);
    }

    // Create "Feed Me!" button
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(btn, feed_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Feed Me!");
    lv_obj_center(btn_label);

    // Initial update
    display_update_mood();

    display_blanking_off(display_dev);
}