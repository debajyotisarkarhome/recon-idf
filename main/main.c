#include <stdio.h>
#include "config.h"
#include "mpu.h"
#include "motor_driver.h"

mpu6050_handle_t mpu ;

mpu6050_acce_value_t acce;
mpu6050_gyro_value_t gyro;
complimentary_angle_t complimentary_angle;

void app_main(void)
{
    mpu_init(&mpu);
    calibrate_mpu(&mpu);
    motor_init();
}