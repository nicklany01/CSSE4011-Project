#include "rtc.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(rtc, CONFIG_LOG_DEFAULT_LEVEL);

static const struct device *rtc_dev = DEVICE_DT_GET(DT_ALIAS(rtc));

int rtc_initialise(void)
{
    if (!device_is_ready(rtc_dev)) {
        LOG_ERR("RTC device not ready");
        return -ENODEV;
    }
    
    LOG_INF("RTC initialized successfully");
    return 0;
}

int rtc_set_datetime(uint16_t year, uint8_t month, uint8_t day,
                     uint8_t hour, uint8_t minute, uint8_t second)
{
    struct rtc_time time_set = {
        .tm_year = year - 1900,
        .tm_mon = month - 1,
        .tm_mday = day,
        .tm_hour = hour,
        .tm_min = minute,
        .tm_sec = second
    };

    int ret = rtc_set_time(rtc_dev, &time_set);
    if (ret < 0) {
        LOG_ERR("Failed to set RTC time: %d", ret);
        return ret;
    }

    LOG_INF("RTC set to %04d-%02d-%02d %02d:%02d:%02d",
           year, month, day, hour, minute, second);
    return 0;
}

int rtc_get_datetime(uint16_t *year, uint8_t *month, uint8_t *day,
                     uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    struct rtc_time current_time;
    int ret = rtc_get_time(rtc_dev, &current_time);
    if (ret < 0) {
        LOG_ERR("Failed to get RTC time: %d", ret);
        return ret;
    }

    if (year) *year = current_time.tm_year + 1900;
    if (month) *month = current_time.tm_mon + 1;
    if (day) *day = current_time.tm_mday;
    if (hour) *hour = current_time.tm_hour;
    if (minute) *minute = current_time.tm_min;
    if (second) *second = current_time.tm_sec;

    return 0;
}

void rtc_print_datetime(void)
{
    uint16_t year;
    uint8_t month, day, hour, minute, second;
    
    if (rtc_get_datetime(&year, &month, &day, &hour, &minute, &second) == 0) {
        LOG_INF("Current RTC time: %04d-%02d-%02d %02d:%02d:%02d",
               year, month, day, hour, minute, second);
    }
}