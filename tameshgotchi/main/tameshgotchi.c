#include "bsp/m5stack_core_2.h"
#define LV_IMG_CACHE_DEF_SIZE 5

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "bsp/esp-bsp.h"

#include "scenes.h"

void app_main() {

	bsp_display_start();
	bsp_display_backlight_on();
	bsp_display_unlock();

	scenes_init();
	scenes_draw();

	mod_mood_e mood = MOD_MOOD_NEUTRAL;

	int scene_tick = 0;
	int cycle_tick = 0;

	while (true) {

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 2; j++) {
				vTaskDelay(pdMS_TO_TICKS(1000));

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
