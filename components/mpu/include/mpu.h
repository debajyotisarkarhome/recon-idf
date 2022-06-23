#include "math.h"
#include "mpu6050.h"
#include "config.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void calibrate_gyro(mpu6050_handle_t *mpu);
void mpu_init(mpu6050_handle_t *mpu);
void stabilize(mpu6050_handle_t *mpu);

