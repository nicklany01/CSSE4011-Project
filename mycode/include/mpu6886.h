#ifndef MPU6886_H
#define MPU6886_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>

#define MPU6886_ADDR 0x68

typedef struct {
    float x;
    float y;
    float z;
} mpu6886_accel_t;

int mpu6886_init(const struct i2c_dt_spec *i2c_dev);
int mpu6886_read_accel(const struct i2c_dt_spec *i2c_dev, mpu6886_accel_t *accel);
// Calculate the magnitude of the accelerometer vector, with z = 1g assumed to be the reference
float get_adjusted_accel_magnitude(const mpu6886_accel_t *accel);

#endif // MPU6886_H