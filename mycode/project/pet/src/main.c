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

#include "friends.h"
#include "journal.h"
#include "scenes.h"
#include "comms.h"
#include "os_ble.h"
#include "os_uart.h"
#include "personality.h"

#define GAME_EVT_JOURNAL_RX_DONE 0x01
#define GAME_EVT_OTHER_PET_CONNECTED 0x02
#define GAME_EVT_OTHER_PET_RSSI 0x04
#define GAME_EVT_RTC_UPDATED 0x08

#define GAME_EVTS_ALL GAME_EVT_JOURNAL_RX_DONE | GAME_EVT_OTHER_PET_CONNECTED | GAME_EVT_OTHER_PET_RSSI | GAME_EVT_RTC_UPDATED

struct k_timer comms_state_timer;
struct k_event game_event_block;

pet_exchange_state_pkt_s pex_state_pkt;
pet_exchange_state_pkt_s their_pex_state_pkt;

pet_exchange_journal_evt_pkt_s my_pex_journal_evt_pkt;
pet_exchange_journal_evt_pkt_s their_pex_journal_evt_pkt;

pet_personality_pkt_s my_pet_ppy_pkt;
pet_personality_pkt_s their_pet_ppy_pkt;

pet_wfc_pkt_demo_cmd_s pet_wfc_demo_pkt;
pet_wfc_rtc_pkt_s pet_wfc_rtc_pkt;
pet_wfc_weather_pkt_s pet_wfc_weather_pkt;

pet_uart_srvc_rssi_pkt_s pet_uart_srvc_rssi_pkt;

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
	my_pet_ppy_pkt.id = pet_pex_id;

	my_pet_ppy_pkt.fav_drink = pet_favs.drink;
	my_pet_ppy_pkt.fav_food = pet_favs.food;
	my_pet_ppy_pkt.fav_scene = pet_favs.scene;
	my_pet_ppy_pkt.fav_temp = pet_favs.temp;
	my_pet_ppy_pkt.fav_time = pet_favs.time;

	memcpy(my_pet_ppy_pkt.weights, pet_personality_weights, PET_ATTR_NEG_MAX);
}

void update_personality() {

	pet_favs.drink = their_pet_ppy_pkt.fav_drink;
	pet_favs.food = their_pet_ppy_pkt.fav_food;
	pet_favs.scene = their_pet_ppy_pkt.fav_scene;
	pet_favs.temp = their_pet_ppy_pkt.fav_temp;
	pet_favs.time = their_pet_ppy_pkt.fav_time;

	memcpy(pet_personality_weights, their_pet_ppy_pkt.weights, PET_ATTR_NEG_MAX);

	// update my_pet_ppy_pkt
	init_personality();
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

	uart_passthru_rx.len = serialize_pet_personality_pkt(&my_pet_ppy_pkt, uart_passthru_rx.buff);
	os_uart_passthru(&uart_passthru_rx);
}

void process_ble_passthru_packet() {

	switch (uart_passthru_rx.buff[0]) {

		case PET_PKT_WFC_DEMO_COMMAND:
			deserialize_pet_wfc_demo_cmd_pkt(&pet_wfc_demo_pkt, uart_passthru_rx.buff);

			switch (pet_wfc_demo_pkt.cmd_id) {

				case PET_WFC_CMD_CHANGE_SCENE:
					scenes_set_main(pet_wfc_demo_pkt.cmd_arg);
					break;
				case PET_WFC_CMD_CHANGE_MOOD:
					scenes_set_mood(pet_wfc_demo_pkt.cmd_arg);
					break;
				case PET_WFC_CMD_CHANGE_TIME:
					scenes_set_time(pet_wfc_demo_pkt.cmd_arg);
					break;

				case PET_WFC_CMD_ADD_FRIEND:
					if (pet_wfc_demo_pkt.cmd_arg == PEX_ID_RESERVED_NULL) {
						// requests our friends list

						break;
					}

					friends_add_friend(pet_wfc_demo_pkt.cmd_arg);
					break;
				case PET_WFC_CMD_REM_FRIEND:
					friends_rem_friend(pet_wfc_demo_pkt.cmd_arg);
					break;
				case PET_WFC_CMD_ADD_ENEMY:
					if (pet_wfc_demo_pkt.cmd_arg == PEX_ID_RESERVED_NULL) {
						// requests our enemies list

						break;
					}

					friends_add_enemy(pet_wfc_demo_pkt.cmd_arg);
					break;
				case PET_WFC_CMD_REM_ENEMY:
					friends_rem_friend(pet_wfc_demo_pkt.cmd_arg);
					break;

				default:
					break;
			}

		case PET_PKT_WFC_WEATHER_UPDATE:
			deserialize_pet_wfc_weather_pkt(&pet_wfc_weather_pkt, uart_passthru_rx.buff);

			scenes_set_weather(pet_wfc_weather_pkt.weather);
			scenes_set_temp_from_int_c(pet_wfc_weather_pkt.temp_c);
			break;
		case PET_PKT_WFC_RTC_UPDATE:
			deserialize_pet_wfc_rtc_pkt(&pet_wfc_rtc_pkt, uart_passthru_rx.buff);

			break;

		case PET_PKT_PPY_PERSONALITY:
			deserialize_pet_personality_pkt(&their_pet_ppy_pkt, uart_passthru_rx.buff);

			if (their_pet_ppy_pkt.id == SIENNA_MF_ID) {
				// request came from da club, update my personality
				update_personality();
				break;
			}

			// other pet's personality, do something with it
			break;

		case PET_PKT_PEX_STATE:
			deserialize_pet_exchange_state_pkt(&their_pex_state_pkt, uart_passthru_rx.buff);

			// other pet's state, do something with it
			break;

		case PET_PKT_PEX_JOURNAL_EVT:
			deserialize_pet_exchange_journal_evt_pkt(&their_pex_journal_evt_pkt, uart_passthru_rx.buff);

			if (their_pex_journal_evt_pkt.index == JOURNAL_REQUEST_MAGIC_NUM) {
				// send my journal
				for (int i = 0; i < journal_idx; i++) {

					my_pex_journal_evt_pkt.index = i == (journal_idx - 1)
						? JOURNAL_FINISHED_MAGIC_NUM
						: i;

					journal_dupe_entry(&my_pex_journal_evt_pkt.entry, &journal[i]);

					uart_passthru_tx.len = serialize_pet_exchange_journal_evt_pkt(
						&my_pex_journal_evt_pkt,  uart_passthru_tx.buff);
					os_uart_passthru(&uart_passthru_tx);
				}

				break;
			}

			// partner's journal, let's add it
			journal_partner_add_entry(&their_pex_journal_evt_pkt.entry);
			if (their_pex_journal_evt_pkt.index == JOURNAL_FINISHED_MAGIC_NUM) {
				k_event_post(&game_event_block, GAME_EVT_JOURNAL_RX_DONE);
			}
			break;

		case PET_PKT_UART_RSSI:
			deserialize_pet_uart_srvc_rssi_pkt(&pet_uart_srvc_rssi_pkt, uart_passthru_rx.buff);
			k_event_post(&game_event_block, GAME_EVT_OTHER_PET_RSSI);

			break;

		default:
			break;
	}
}

/* <-- UART_HANDLER -->
 *
 * responsible for managing UART comms with the uplink
 * and scene updates after one-shot operations
 *
 * own's the partner's journal
 *
 * WARNING: no other thread should add to partner's journal
 * WARNING: no other thread should rx/tx UART */

K_THREAD_STACK_DEFINE(stack_uart_handler, 2048);
struct k_thread thread_uart_handler_data;

static void thread_uart_handler(void *a, void *b, void *c) {


	k_timer_init(&comms_state_timer, comms_state_timeout, NULL);
	k_timer_start(&comms_state_timer, K_SECONDS(3), K_SECONDS(3));

	while (true) {

		k_msgq_get(&os_uart_rxq, &uart_passthru_rx, K_FOREVER);
		process_ble_passthru_packet();
	}
}

/* <-- GAME_HANDLER -->

 * responsible for managing journal entries
 * and scene updates after long-running operations
 *
 * own's my journal
 *
 * WARNING: no other thread should add to journal */

K_THREAD_STACK_DEFINE(stack_game_handler, 2048);
struct k_thread thread_game_handler_data;

static void thread_game_handler(void *a, void *b, void *c) {

	uint32_t game_events = 0;

	// start our day :)
	journal_add_entry(JOURNAL_EVT_WAKE);

	while (true) {

		game_events = k_event_wait(&game_event_block, GAME_EVTS_ALL, true, K_FOREVER);

		if (game_events & GAME_EVT_JOURNAL_RX_DONE) {
			// should display the appropriate screen here
		}

		if (game_events & GAME_EVT_OTHER_PET_CONNECTED) {
			journal_add_entry(JOURNAL_EVT_VISIT);
		}

		if (game_events & GAME_EVT_OTHER_PET_RSSI) {

			switch (friends_pex_id_is_what(pet_uart_srvc_rssi_pkt.id)) {
				case FRIENDSHIP_FRIEND:
					journal_add_entry(JOURNAL_EVT_PROX_FRIEND);
					break;
				case FRIENDSHIP_ENEMY:
					journal_add_entry(JOURNAL_EVT_PROX_ENEMY);
					break;

				default:
					break;
			}
		}

		if (game_events & GAME_EVT_RTC_UPDATED) {
			// check current time and if diff to last then
			// add journal for sleepy or whatever
		}
	}
}

int main() {

	const struct device *display_dev;
	k_tid_t tid_uart_handler, tid_game_handler;

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

	k_event_init(&game_event_block);

	friends_add_friend(0xBAB0);

	tid_uart_handler = k_thread_create(
		&thread_uart_handler_data,
		stack_uart_handler,
		K_THREAD_STACK_SIZEOF(stack_uart_handler),
		thread_uart_handler,
		NULL, NULL, NULL,
		2, 0, K_NO_WAIT
	);

	tid_game_handler = k_thread_create(
		&thread_game_handler_data,
		stack_game_handler,
		K_THREAD_STACK_SIZEOF(stack_game_handler),
		thread_game_handler,
		NULL, NULL, NULL,
		2, 0, K_NO_WAIT
	);

	scenes_init();
	scenes_draw();

	display_blanking_off(display_dev);

	while (1) {

		// main thread's only job is
		// screen refresh 2Hz
		scenes_draw();
		k_sleep(K_MSEC(500));
	}
}
/*
=======
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include "sound.h"
#include "mood.h"
#include <lvgl_input_device.h>
#include <zephyr/shell/shell.h>
#include "rtc.h"

void main(void)
{
    const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    
    mood_init();
    sound_init();
    mpu6886_init();
    display_init(display_dev);

    while (1) {
        lv_timer_handler();
        k_sleep(K_MSEC(10));
    }
}

K_THREAD_DEFINE(mood_thread_id, MOOD_THREAD_STACK_SIZE, mood_thread, NULL, NULL, NULL, MOOD_THREAD_PRIORITY, 0, 0);
<<<<<<< HEAD

static int cmd_set_time(const struct shell *shell, size_t argc, char **argv)
{
    if (argc != 7) {
        shell_error(shell, "Usage: set_time <year> <month> <day> <hour> <min> <sec>");
        return -EINVAL;
    }

    uint16_t year = atoi(argv[1]);
    uint8_t month = atoi(argv[2]);
    uint8_t day = atoi(argv[3]);
    uint8_t hour = atoi(argv[4]);
    uint8_t minute = atoi(argv[5]);
    uint8_t second = atoi(argv[6]);

    int ret = rtc_set_datetime(year, month, day, hour, minute, second);
    if (ret < 0) {
        shell_error(shell, "Failed to set time: %d", ret);
        return ret;
    }

    shell_print(shell, "Time set successfully");
    return 0;
}

static int cmd_get_time(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    uint16_t year;
    uint8_t month, day, hour, minute, second;

    int ret = rtc_get_datetime(&year, &month, &day, &hour, &minute, &second);
    if (ret < 0) {
        shell_error(shell, "Failed to get RTC time: %d", ret);
        return ret;
    }

    shell_print(shell, "Current time: %04d-%02d-%02d %02d:%02d:%02d",
                year, month, day, hour, minute, second);

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    rtc_cmds,
    SHELL_CMD(set, NULL, "Set RTC time (year month day hour min sec)", cmd_set_time),
    SHELL_CMD(get, NULL, "Get current RTC time", cmd_get_time),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(rtc, &rtc_cmds, "RTC commands", NULL);
=======
>>>>>>> Nick
*/
