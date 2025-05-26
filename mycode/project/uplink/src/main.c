#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>

#include "os_ble.h"

int main() {

	os_ble_init();

	while (1) {

		k_sleep(K_MSEC(1000));

	}
}
