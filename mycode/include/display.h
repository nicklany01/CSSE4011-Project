#ifndef DISPLAY_H
#define DISPLAY_H

#include <zephyr/drivers/display.h>

void display_init(const struct device *display_dev);

#endif // DISPLAY_H