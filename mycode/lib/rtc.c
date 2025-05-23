
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/logging/log.h>
#include <soc.h>
#include <em_rtcc.h>

const struct device *rtc = DEVICE_DT_GET(DT_NODELABEL(rtcc0));

struct time
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
};

void set_rtcc_to_calendar_mode(void)
{
	RTCC->CTRL &= ~_RTCC_CTRL_CNTMODE_MASK;
	RTCC->CTRL |= _RTCC_CTRL_CNTMODE_CALENDAR;

	printk("RTCC mode set to Calendar Mode\n");
}

void init_rtcc(void)
{
	if (!device_is_ready(rtc))
	{
		printk("RTCC not ready\n");
		return;
	}

	// set_rtcc_to_calendar_mode();
	counter_start(rtc);
}

void set_rtcc_time(struct time t)
{
	RTCC->CNT = (t.hours * 3600 + t.minutes * 60 + t.seconds) * counter_get_frequency(rtc);
}

struct time get_rtcc_time(void)
{
	struct time current_time;
	uint32_t ticks;
	uint32_t freq = counter_get_frequency(rtc);
	counter_get_value(rtc, &ticks);

	uint32_t seconds_total = ticks / freq;
	current_time.hours = (seconds_total / 3600) % 24;
	current_time.minutes = (seconds_total / 60) % 60;
	current_time.seconds = seconds_total % 60;

	return current_time;
}

uint32_t get_rtcc_seconds(void)
{
	uint32_t ticks;
	uint32_t freq = counter_get_frequency(rtc);
	counter_get_value(rtc, &ticks);
	return ticks / freq;
}