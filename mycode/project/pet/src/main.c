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
