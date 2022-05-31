#include <stdio.h>
#include "config.h"
#include "mpu.h"

mpu6050_acce_value_t acce;
mpu6050_gyro_value_t gyro;
complimentary_angle_t complimentary_angle;

void app_main(void)
{
    mpu_init();
    calibrate_mpu();
    const esp_timer_create_args_t cal_timer_config = {
        .callback = calc_mpu,
        .arg = NULL,
        .name = "MPU6050 timer",
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
        .skip_unhandled_events = true,
    #endif
        .dispatch_method = ESP_TIMER_TASK
    };
    esp_timer_handle_t cal_timer = NULL;
    esp_timer_create(&cal_timer_config, &cal_timer);
    esp_timer_start_periodic(cal_timer, 5000); // 5ms


}