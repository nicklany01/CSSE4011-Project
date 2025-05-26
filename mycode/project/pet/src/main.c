#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>

#include "scenes.h"
#include "os_ble.h"

int main() {

	const struct device *display_dev;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		return 0;
	}

	scenes_init();
	scenes_draw();

	lv_timer_handler();
	display_blanking_off(display_dev);

	if (!os_ble_init()) {
		scenes_set_main(MAIN_SCENE_BEACH);
		scenes_set_time(MOD_TIME_NIGHT);
		scenes_set_mood(MOD_MOOD_ANGRY);
		scenes_draw();
		return;
	}

	while (1) {

		if (os_ble_evt_flags & OS_BLE_EVT_MSK_UPLINK) {
			scenes_set_main(MAIN_SCENE_SHOP);
			scenes_set_mood(MOD_MOOD_HAPPY);
			scenes_draw();
		} else if (os_ble_evt_flags & OS_BLE_EVT_MSK_ADV_RX) {
			scenes_set_main(MAIN_SCENE_BEACH);
			scenes_set_mood(MOD_MOOD_SAD);
			scenes_draw();
		}

		k_sleep(K_MSEC(500));
	}
}
