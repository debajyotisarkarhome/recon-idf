#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/mcpwm.h"
#include "config.h"

#define MOTOR1 4
#define MOTOR2 2
#define MOTOR3 14
#define MOTOR4 12