#include "mpu6050.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void calibrate_mpu(mpu6050_handle_t *mpu);
void mpu_init(mpu6050_handle_t *mpu);

