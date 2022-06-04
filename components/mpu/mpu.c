#include <stdio.h>
#include "mpu.h"
#include "config.h"

esp_err_t error = ESP_OK;
mpu6050_acce_value_t acce_local;
mpu6050_gyro_value_t gyro_local;
complimentary_angle_t complimentary_angle_local;

float pitch_error = 0.0 ;
float roll_error = 0.0 ;

void calibrate_mpu(mpu6050_handle_t *mpu){
    if (*mpu == NULL) {
        ESP_LOGE(TAG, "MPU6050 handle is NULL");
        return;
    }
    float pitch_sum = 0;
    float roll_sum = 0;
    for(int c=0;c<calibrate_count;c++){
        mpu6050_get_acce(*mpu, &acce_local);
        mpu6050_get_gyro(*mpu, &gyro_local);
        mpu6050_complimentory_filter(*mpu, &acce_local, &gyro_local, &complimentary_angle_local);
        pitch_sum += complimentary_angle_local.pitch;
        roll_sum += complimentary_angle_local.roll;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    pitch_error = pitch_sum/calibrate_count;
    roll_error = roll_sum/calibrate_count;
    if(abs(pitch_error)>10 || abs(roll_error)>10){
        ESP_LOGI(TAG, "MPU calibration error, please level the board | debug angles : %f %f", pitch_error, roll_error);
        calibrate_mpu(mpu);
    }
    else{
        ESP_LOGI(TAG, "MPU Calibrated successfully | debug angles : %f %f", pitch_error, roll_error);
    }
}

void mpu_init(mpu6050_handle_t *mpu){
    int i2c_master_port = I2C_NUM_1;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_13,         //sda
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = GPIO_NUM_15,         //scl
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,  
    };
    error = i2c_param_config(i2c_master_port, &conf);
    if(error != ESP_OK){
        ESP_LOGE(TAG, "I2C Param Config failed");
        return;
    }
    error = i2c_driver_install(i2c_master_port, conf.mode,0,0,0);
    if(error != ESP_OK){
        ESP_LOGE(TAG, "I2C Driver Install failed");
        return;
    }
    *mpu=mpu6050_create(i2c_master_port,0x68u);
    mpu6050_wake_up(*mpu);
    error = mpu6050_config(*mpu, ACCE_FS_4G, GYRO_FS_500DPS);
    if(error != ESP_OK){
        ESP_LOGE(TAG, "MPU Config failed");
        *mpu=NULL;
        return;
    }
    ESP_LOGI(TAG,"MPU6050 initiated successfully");
}

