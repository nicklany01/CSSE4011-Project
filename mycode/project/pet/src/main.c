#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/fs/fs.h>

#include "scenes.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL

int main() {

	const struct device *display_dev;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return 0;
	}

	scenes_init();
	scenes_draw();

	mod_mood_e mood = MOD_MOOD_NEUTRAL;

	int scene_tick = 0;
	int cycle_tick = 0;

	lv_timer_handler();
	display_blanking_off(display_dev);

	while (1) {

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 2; j++) {
				k_sleep(K_MSEC(1000));

				if (j == 0) {
					scenes_set_time(MOD_TIME_MIDDAY);
				} else if (scenes_state.main_scene != MAIN_SCENE_SHOP) {
					scenes_set_time(MOD_TIME_NIGHT);
				}

				if (i == 0) {
					scenes_set_main(MAIN_SCENE_MEADOW);
				} else if (i == 2) {
					scenes_set_main(MAIN_SCENE_FOREST);
				} else if (i == 3) {
					scenes_set_main(MAIN_SCENE_BEACH);
				} else {
					scenes_set_main(MAIN_SCENE_SHOP);
				}

				mood = mood == MOD_MOOD_MAX
					? 0
					: mood + 1;

				scenes_set_mood(mood);

				scenes_draw();
			}
		}
	}
}
