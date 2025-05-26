#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/drivers/hwinfo.h>

#include "scenes.h"
#include "comms.h"
#include "os_ble.h"
#include "os_uart.h"
#include "personality.h"

struct k_timer comms_state_timer;

pet_exchange_state_pkt_s pex_state_pkt;
pet_personality_pkt_s pet_ppy_pkt;

os_uart_passthru_s uart_passthru_rx = {
	.len = -1,
	.buff = {0}
};

os_uart_passthru_s uart_passthru_tx = {
	.len = -1,
	.buff = {0}
};

void init_personality() {

	//hwinfo_get_device_id((uint8_t *)pet_pex_id, sizeof(pex_uuid_t));

	pet_pex_id = 0xBABE;
	pet_ppy_pkt.id = pet_pex_id;

	pet_ppy_pkt.fav_drink = pet_favs.drink;
	pet_ppy_pkt.fav_food = pet_favs.food;
	pet_ppy_pkt.fav_scene = pet_favs.scene;
	pet_ppy_pkt.fav_temp = pet_favs.temp;
	pet_ppy_pkt.fav_time = pet_favs.time;

	memcpy(pet_ppy_pkt.weights, pet_personality_weights, PET_ATTR_NEG_MAX);
}

void comms_state_timeout(struct k_timer *timer) {

	pex_state_pkt.scene = scenes_state.main_scene;
	pex_state_pkt.scene_weather = scenes_state.modifier_weather;
	pex_state_pkt.scene_mood = scenes_state.modifier_mood;
	pex_state_pkt.scene_time = scenes_state.modifier_time;
	pex_state_pkt.scene_temp = scenes_state.modifier_temp;

	pex_state_pkt.held_drink = scenes_state.held_drink;
	pex_state_pkt.held_food = scenes_state.held_food;

	uart_passthru_rx.len = serialize_pet_exchange_state_pkt(&pex_state_pkt, uart_passthru_rx.buff);
	os_uart_passthru(&uart_passthru_rx);

	uart_passthru_rx.len = serialize_pet_personality_pkt(&pet_ppy_pkt, uart_passthru_rx.buff);
	os_uart_passthru(&uart_passthru_rx);
}

int main() {

	const struct device *display_dev;
	uint8_t uart_tx_buff[RX_BUFF_SIZE] = "goodbye!";

	k_timer_init(&comms_state_timer, comms_state_timeout, NULL);

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		return 0;
	}

	init_personality();

	if (!os_ble_init()) {
		return;
	}

	if (!os_uart_init()) {
		return;
	};

	scenes_init();
	scenes_draw();

	lv_timer_handler();
	display_blanking_off(display_dev);

	k_timer_start(&comms_state_timer, K_SECONDS(3), K_SECONDS(3));

	while (1) {

		if (k_msgq_num_used_get(&os_uart_rxq) > 0) {
			k_msgq_get(&os_uart_rxq, &uart_passthru_rx, K_NO_WAIT);
		}

		k_sleep(K_MSEC(100));
	}
}
