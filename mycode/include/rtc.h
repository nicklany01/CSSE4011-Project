#ifndef RTC_H
#define RTC_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/rtc.h>

int rtc_initialise(void);
int rtc_set_datetime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
int rtc_get_datetime(int *year, int *month, int *day, int *hour, int *minute, int *second);
void rtc_print_datetime(void);

#endif /* RTC_H */
