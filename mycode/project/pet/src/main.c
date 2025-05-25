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
#include "mood.h"
#include <lvgl_input_device.h>

void main(void)
{
    const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    
    mood_init();
    sound_init();
    display_init(display_dev);

    while (1) {
        lv_timer_handler();
        k_sleep(K_MSEC(10));
    }
}

K_THREAD_DEFINE(mood_thread_id, MOOD_THREAD_STACK_SIZE, mood_thread, NULL, NULL, NULL, MOOD_THREAD_PRIORITY, 0, 0);