#include <stdio.h>
#include "config.h"
#include "mpu.h"
#include "motor_driver.h"

mpu6050_handle_t mpu ;

void app_main(void)
{
    mpu_init(&mpu);
    calibrate_gyro(&mpu);
    motor_init();  
    stabilize(&mpu);
}