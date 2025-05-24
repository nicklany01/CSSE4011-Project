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

LOG_MODULE_REGISTER(pet);

#define MAX_PATH_LEN 255
#define MOUNT_POINT "/lfs1"

static uint32_t count;
static bool sound_playing = false;

static struct gpio_dt_spec button_gpio = GPIO_DT_SPEC_GET_OR(
        DT_ALIAS(sw0), gpios, {0});
static struct gpio_callback button_callback;

static lv_obj_t *hello_world_label;
static lv_obj_t *count_label;
static lv_obj_t *sound_button;
static lv_obj_t *sound_status_label;

static int sound_init_status = -1;

static void button_isr_callback(const struct device *port,
                struct gpio_callback *cb,
                uint32_t pins)
{
    ARG_UNUSED(port);
    ARG_UNUSED(cb);
    ARG_UNUSED(pins);

    if (!sound_playing && sound_init_status == 0) {
        sound_playing = true;
        lv_label_set_text(sound_status_label, "Playing...");
    } else {
        sound_playing = false;
        lv_label_set_text(sound_status_label, "Press to play");
    }
}

static void sound_btn_callback(lv_event_t *e)
{
    ARG_UNUSED(e);
    
    if (!sound_playing && sound_init_status == 0) {
        sound_playing = true;
        lv_label_set_text(sound_status_label, "Playing...");
    } else {
        sound_playing = false;
        lv_label_set_text(sound_status_label, "Press to play");
    }
}

void init_filesystem(void)
{
    struct fs_file_t file;
    char path[MAX_PATH_LEN];
    int rc;

    snprintf(path, sizeof(path), "%s/hello.txt", MOUNT_POINT);
    fs_file_t_init(&file);

    rc = fs_open(&file, path, FS_O_CREATE | FS_O_WRITE);
    if (rc < 0) {
        LOG_ERR("Failed to create file: %d", rc);
        return;
    }

    rc = fs_write(&file, "Hello World!", strlen("Hello World!"));
    if (rc < 0) {
        LOG_ERR("Failed to write to file: %d", rc);
        fs_close(&file);
        return;
    }

    LOG_INF("File created and written successfully");
    fs_close(&file);
}

void setup_display(const struct device *display_dev)
{
    hello_world_label = lv_label_create(lv_screen_active());
    lv_label_set_text(hello_world_label, "Sound Player");
    lv_obj_align(hello_world_label, LV_ALIGN_TOP_MID, 0, 20);

    sound_button = lv_button_create(lv_screen_active());
    lv_obj_align(sound_button, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_size(sound_button, 150, 50);
    lv_obj_add_event_cb(sound_button, sound_btn_callback, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *btn_label = lv_label_create(sound_button);
    lv_label_set_text(btn_label, "Play Sound");
    lv_obj_center(btn_label);

    sound_status_label = lv_label_create(lv_screen_active());
    lv_label_set_text(sound_status_label, "Press to play");
    lv_obj_align(sound_status_label, LV_ALIGN_CENTER, 0, 30);

    count_label = lv_label_create(lv_screen_active());
    lv_obj_align(count_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    
    display_blanking_off(display_dev);
}

void setup_button(void)
{
    if (!gpio_is_ready_dt(&button_gpio)) {
        LOG_WRN("Button not ready");
        return;
    }

    int err = gpio_pin_configure_dt(&button_gpio, GPIO_INPUT);
    if (err) {
        LOG_ERR("failed to configure button gpio: %d", err);
        return;
    }

    gpio_init_callback(&button_callback, button_isr_callback,
               BIT(button_gpio.pin));

    err = gpio_add_callback(button_gpio.port, &button_callback);
    if (err) {
        LOG_ERR("failed to add button callback: %d", err);
        return;
    }

    err = gpio_pin_interrupt_configure_dt(&button_gpio,
                          GPIO_INT_EDGE_TO_ACTIVE);
    if (err) {
        LOG_ERR("failed to enable button callback: %d", err);
        return;
    }
}

int main(void)
{
    const struct device *display_dev;
    char count_str[11] = {0};

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Display device not ready");
        return 0;
    }

    init_filesystem();

    sound_init_status = sound_init();
    if (sound_init_status < 0) {
        LOG_ERR("Sound initialization failed: %d", sound_init_status);
    }

    setup_display(display_dev);

    setup_button();

    while (1) {
        if ((count % 100) == 0U) {
            sprintf(count_str, "%d", count/100U);
            lv_label_set_text(count_label, count_str);
        }

        if (sound_playing && sound_init_status == 0) {
            int ret = sound_play();
            if (ret < 0) {
                LOG_ERR("Sound playback failed: %d", ret);
                sound_playing = false;
                lv_label_set_text(sound_status_label, "Playback error");
            }
        }

        lv_timer_handler();
        ++count;
        k_sleep(K_MSEC(10));
    }

    sound_cleanup();
    return 0;
}