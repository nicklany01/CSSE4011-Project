// #include <zephyr/kernel.h>
// #include <zephyr/logging/log.h>
// #include <lvgl.h>
// #include "mood.h"
// #include "display.h"
// #include "sound.h"

// LOG_MODULE_REGISTER(display, CONFIG_LOG_DEFAULT_LEVEL);

// static lv_obj_t *bars[5];
// static lv_obj_t *labels[5];
// static lv_obj_t *expression_label;  // New label for expression

// void display_update_mood(void)
// {
//     const char *names[] = {"Affection", "Happiness", "Energy", "Health", "Interaction"};
//     struct mood_state local_copy;
    
//     // Safely copy mood state
//     if(k_mutex_lock(&mood_mutex, K_MSEC(10)) == 0) {
//         memcpy(&local_copy, &pet_mood, sizeof(local_copy));
//         k_mutex_unlock(&mood_mutex);
//     } else {
//         LOG_WRN("Mood update skipped (mutex busy)");
//         return;
//     }

//     for (int i = 0; i < 5; i++) {
//         int value;
//         switch (i) {
//             case 0: value = local_copy.affection; break;
//             case 1: value = local_copy.happiness; break;
//             case 2: value = local_copy.energy; break;
//             case 3: value = local_copy.health; break;
//             case 4: value = local_copy.interaction; break;
//             default: value = 0; break;
//         }

//         lv_bar_set_value(bars[i], value / 10, LV_ANIM_ON);
//         char buf[32];
//         snprintf(buf, sizeof(buf), "%s: %d", names[i], value);
//         lv_label_set_text(labels[i], buf);
//     }

//     if (local_copy.expression >= EXPRESSION_ENLIGHTENED && 
//         local_copy.expression <= EXPRESSION_SLEEPY) {
//         lv_label_set_text(expression_label, 
//                          expression_strings[local_copy.expression]);
//     } else {
//         lv_label_set_text(expression_label, "UNKNOWN");
//     }
// }

// static void feed_button_cb(lv_event_t *e)
// {
//     ARG_UNUSED(e);
    
//     if (k_mutex_lock(&mood_mutex, K_MSEC(10)) == 0) {
//         pet_mood.energy = MIN(pet_mood.energy + 100, MAX_STATE_VALUE);
//         k_mutex_unlock(&mood_mutex);
        
//         LOG_INF("Fed pet! Energy: %d", pet_mood.energy);
//         sound_play();
//         display_update_mood();
//     } else {
//         LOG_WRN("Couldn't feed - mood system busy");
//     }
// }

// void display_init(const struct device *display_dev)
// {
//     // container for mood bars
//     lv_obj_t *cont = lv_obj_create(lv_scr_act());
//     lv_obj_set_size(cont, LV_PCT(90), LV_PCT(60));  // Reduced height for expression
//     lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 20);
//     lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
//     lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
//     // bars and labels
//     const char *names[] = {"Affection", "Happiness", "Energy", "Health", "Interaction"};
//     for (int i = 0; i < 5; i++) {
//         lv_obj_t *bar_cont = lv_obj_create(cont);
//         lv_obj_set_size(bar_cont, LV_PCT(18), LV_PCT(100));
//         lv_obj_set_style_pad_all(bar_cont, 5, 0);
        
//         bars[i] = lv_bar_create(bar_cont);
//         lv_obj_set_size(bars[i], LV_PCT(100), LV_PCT(70));
//         lv_bar_set_range(bars[i], 0, 100);
//         lv_obj_align(bars[i], LV_ALIGN_TOP_MID, 0, 0);
        
//         labels[i] = lv_label_create(bar_cont);
//         lv_label_set_text(labels[i], names[i]);
//         lv_obj_align(labels[i], LV_ALIGN_BOTTOM_MID, 0, 0);
//     }

//     // expression label
//     expression_label = lv_label_create(lv_scr_act());
//     lv_obj_align(expression_label, LV_ALIGN_TOP_MID, 0, 0);
//     lv_label_set_text(expression_label, "NEUTRAL");

//     // feed me button
//     lv_obj_t *btn = lv_btn_create(lv_scr_act());
//     lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
//     lv_obj_add_event_cb(btn, feed_button_cb, LV_EVENT_CLICKED, NULL);
    
//     lv_obj_t *btn_label = lv_label_create(btn);
//     lv_label_set_text(btn_label, "Feed Me!");
//     lv_obj_center(btn_label);

//     display_update_mood();
//     display_blanking_off(display_dev);
// }