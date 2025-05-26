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
#include "os_uart.h"

int main() {

	const struct device *display_dev;
	uint8_t uart_tx_buff[RX_BUFF_SIZE] = "goodbye!";

	os_uart_passthru_s passthru_rx;

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

	if (!os_uart_init()) {
		scenes_set_main(MAIN_SCENE_BEACH);
		scenes_set_time(MOD_TIME_NIGHT);
		scenes_set_mood(MOD_MOOD_ANGRY);
		scenes_draw();
		return;
	};

	while (1) {

		/*if (os_ble_evt_flags & OS_BLE_EVT_MSK_UPLINK) {

		} else if (os_ble_evt_flags & OS_BLE_EVT_MSK_ADV_RX) {
			scenes_set_main(MAIN_SCENE_BEACH);
			scenes_set_mood(MOD_MOOD_SAD);
			scenes_draw();
		}*/

		if (k_msgq_num_used_get(&os_uart_rxq) > 0) {
			k_msgq_get(&os_uart_rxq, &passthru_rx, K_NO_WAIT);

			os_uart_tx(passthru_rx.buff, passthru_rx.len);

			scenes_set_main(MAIN_SCENE_SHOP);
			scenes_set_mood(MOD_MOOD_HAPPY);
			scenes_draw();
		}

		k_sleep(K_MSEC(100));
	}
}
