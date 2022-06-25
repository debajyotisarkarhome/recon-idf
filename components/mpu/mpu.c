#include <stdio.h>
#include "mpu.h"

esp_err_t error = ESP_OK;
mpu6050_acce_value_t acce_local;
mpu6050_gyro_value_t gyro_local;


double pitch_error = 0.0 ;
double roll_error = 0.0 ;

double gyro_x_error = 0.0 ; 
double gyro_y_error = 0.0 ;
double gyro_z_error = 0.0 ;

double gyro_x;
double gyro_y;
double gyro_z;

double acc_x;
double acc_y;
double acc_z;

double angle_pitch;
double angle_roll;

double acc_total_vector;
double angle_pitch_acc;
double angle_roll_acc;

// double yaw_test ; //test

int set_gyro_angles = 0;

void calibrate_gyro(mpu6050_handle_t *mpu){
    if (*mpu == NULL) {
        ESP_LOGE(TAG, "MPU6050 handle is NULL");
        return;
    }
    double gyro_x_sum = 0;
    double gyro_y_sum = 0;
    double gyro_z_sum = 0;

    for(int c=0;c<calibrate_count;c++){
        mpu6050_get_gyro(*mpu, &gyro_local);
        gyro_x_sum += gyro_local.gyro_x;
        gyro_y_sum += gyro_local.gyro_y;
        gyro_z_sum += gyro_local.gyro_z;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    gyro_x_error = gyro_x_sum/calibrate_count;
    gyro_y_error = gyro_y_sum/calibrate_count;
    gyro_z_error = gyro_z_sum/calibrate_count;

    // if(abs(gyro_x_error)>10 || abs(gyro_x_error)>10){
    //     ESP_LOGI(TAG, "MPU calibration error, please level the board | debug angles : %f %f", pitch_error, roll_error);
    //     calibrate_mpu(mpu);
    // }
    // else{
    //     ESP_LOGI(TAG, "MPU Calibrated successfully | debug angles : %f %f", pitch_error, roll_error);
    // }
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

void measure(mpu6050_handle_t *mpu){
    mpu6050_get_gyro(*mpu, &gyro_local);
    mpu6050_get_acce(*mpu, &acce_local);

    gyro_x = gyro_local.gyro_x - gyro_x_error;                                                //Subtract the offset calibration value from the raw gyro_x value
    gyro_y = gyro_local.gyro_y - gyro_y_error;                                                //Subtract the offset calibration value from the raw gyro_y value
    // gyro_z = gyro_local.gyro_z - gyro_z_error;

    acc_x = acce_local.acce_x;
    acc_y = acce_local.acce_y;
    acc_z = acce_local.acce_z;

    angle_pitch += gyro_x/100 ;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
    angle_roll += gyro_y/100 ;                                    //Calculate the traveled roll angle and add this to the angle_roll variable

    // angle_pitch += angle_roll * sin(gyro_z * 0.01745329 );
    // angle_roll -= angle_pitch * sin(gyro_z * 0.01745329 );

    acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));
    angle_pitch_acc = asin((double)acc_y/acc_total_vector)* 57.296; 
    angle_roll_acc = asin((double)acc_x/acc_total_vector)* -57.296;

    if(set_gyro_angles){                                                 //If the IMU is already started
        angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
        angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
    }
    else{                                                                //At first start
        angle_pitch = angle_pitch_acc;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
        angle_roll = angle_roll_acc;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
        set_gyro_angles = 1;                                            //Set the IMU started flag
    }

    ESP_LOGI(TAG, "Pitch : %f | Roll : %f", angle_pitch, angle_roll);
}

void stabilize(mpu6050_handle_t *mpu){
    // const esp_timer_create_args_t periodic_timer_args = {
    //         .callback = measure,
    //         .name = "periodic",
    //         .arg = mpu,
    // };
    // esp_timer_handle_t periodic_timer;
    // esp_timer_create(&periodic_timer_args, &periodic_timer);
    // esp_timer_start_periodic(periodic_timer, 500);
    while(1){
        measure(mpu);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
