#include "mpu6886.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(mpu6886, CONFIG_LOG_DEFAULT_LEVEL);

#define MPU6886_WHO_AM_I     0x75
#define MPU6886_PWR_MGMT_1   0x6B
#define MPU6886_ACCEL_XOUT_H 0x3B
#define MPU6886_ACCEL_CONFIG 0x1C

#define MPU6886_WHO_AM_I_ID  0x19

int mpu6886_init(const struct i2c_dt_spec *i2c_dev) {
    uint8_t whoami;
    int ret;

    // Check device ID
    ret = i2c_reg_read_byte_dt(i2c_dev, MPU6886_WHO_AM_I, &whoami);
    if (ret < 0) {
        LOG_ERR("Failed to read WHO_AM_I register");
        return ret;
    }

    if (whoami != MPU6886_WHO_AM_I_ID) {
        LOG_ERR("Invalid WHO_AM_I value: 0x%02x", whoami);
        return -ENODEV;
    }

    // Wake up device
    ret = i2c_reg_write_byte_dt(i2c_dev, MPU6886_PWR_MGMT_1, 0x00);
    if (ret < 0) {
        LOG_ERR("Failed to wake up device");
        return ret;
    }

    // Set accelerometer range to ±8G (0x10)
    ret = i2c_reg_write_byte_dt(i2c_dev, MPU6886_ACCEL_CONFIG, 0x10);
    if (ret < 0) {
        LOG_ERR("Failed to set accelerometer range");
        return ret;
    }

    return 0;
}

int mpu6886_read_accel(const struct i2c_dt_spec *i2c_dev, mpu6886_accel_t *accel) {
    uint8_t buf[6];
    int16_t raw_x, raw_y, raw_z;
    int ret;

    ret = i2c_burst_read_dt(i2c_dev, MPU6886_ACCEL_XOUT_H, buf, sizeof(buf));
    if (ret < 0) {
        LOG_ERR("Failed to read accelerometer data");
        return ret;
    }

    raw_x = (buf[0] << 8) | buf[1];
    raw_y = (buf[2] << 8) | buf[3];
    raw_z = (buf[4] << 8) | buf[5];

    // Convert to g (assuming 8G range)
    accel->x = (float)raw_x / 4096.0f;
    accel->y = (float)raw_y / 4096.0f;
    accel->z = (float)raw_z / 4096.0f;

    return 0;
}

float get_adjusted_accel_magnitude(const mpu6886_accel_t *accel) {
    return sqrtf(accel->x * accel->x + accel->y * accel->y + (accel->z - 1) * (accel->z - 1));
}