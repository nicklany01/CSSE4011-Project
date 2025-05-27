#include "mpu6886.h"
#include <zephyr/device.h>

void main(void) {
    mpu6886_accel_t accel;

    if (!device_is_ready(mpu6886_i2c.bus)) {
        printk("I2C bus not ready\n");
        return;
    }

    if (mpu6886_init() < 0) {
        printk("Failed to initialize MPU6886\n");
        return;
    }

    while (1) {
        if (mpu6886_read_accel(&accel) == 0) {
            printk("X: %.2f, Y: %.2f, Z: %.2f\n", accel.x, accel.y, accel.z);
            float magnitude = mpu6886_get_adjusted_accel_magnitude(&accel);
            printk("Magnitude: %.2f\n", magnitude);
        } else {
            printk("Failed to read accelerometer data\n");
        }
        k_sleep(K_MSEC(100));
    }
}