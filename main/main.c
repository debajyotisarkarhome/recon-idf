#include <stdio.h>
#include "config.h"
#include "mpu6050.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

mpu6050_handle_t mpu = NULL;
mpu6050_acce_value_t acce;
mpu6050_gyro_value_t gyro;
complimentary_angle_t complimentary_angle;

float pitch_error = 0.0 ;
float roll_error = 0.0 ;





void calibrate_mpu(){
    float pitch_sum = 0;
    float roll_sum = 0;
    for(int c=0;c<calibrate_count;c++){
        mpu6050_get_acce(mpu, &acce);
        mpu6050_get_gyro(mpu, &gyro);
        mpu6050_complimentory_filter(mpu, &acce, &gyro, &complimentary_angle);
        pitch_sum += complimentary_angle.pitch;
        roll_sum += complimentary_angle.roll;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    pitch_error = pitch_sum/calibrate_count;
    roll_error = roll_sum/calibrate_count;

}

void mpu_init(){
    int i2c_master_port = I2C_NUM_1;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_13,         //sda
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = GPIO_NUM_15,         //scl
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,  
    };
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,0,0,0);
    mpu=mpu6050_create(i2c_master_port,0x68u);
    mpu6050_wake_up(mpu);
    mpu6050_config(mpu, ACCE_FS_4G, GYRO_FS_500DPS);
}

void calc_mpu(){
    mpu6050_get_acce(mpu, &acce);
    mpu6050_get_gyro(mpu, &gyro);
    mpu6050_complimentory_filter(mpu, &acce, &gyro, &complimentary_angle);
    ESP_LOGI(TAG, "comp : %f %f ; err : %f %f",complimentary_angle.roll-roll_error, complimentary_angle.pitch-pitch_error,roll_error, pitch_error);
}

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