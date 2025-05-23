#ifndef RTCC_TIME_H
#define RTCC_TIME_H

#include <stdint.h>
#include <zephyr/device.h>

/* Time structure */
struct time {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
};

/* Public API */
void set_rtcc_to_calendar_mode(void);
void init_rtcc(void);
void set_rtcc_time(struct time t);
struct time get_rtcc_time(void);

uint32_t get_rtcc_seconds(void);

#endif // RTCC_TIME_H
