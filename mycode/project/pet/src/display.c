#include <zephyr/kernel.h>
#include <zephyr/fs/fs.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include "sound.h"
#include <lvgl_input_device.h>

LOG_MODULE_REGISTER(display, CONFIG_LOG_DEFAULT_LEVEL);

static void button_cb(lv_event_t *e)
{
    ARG_UNUSED(e);
    LOG_INF("Nice");
    sound_play();
}

void display_init(const struct device *display_dev)
{
    lv_obj_t *btn = lv_button_create(lv_screen_active());
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Feed Me!");
    lv_obj_center(label);

    display_blanking_off(display_dev);
}