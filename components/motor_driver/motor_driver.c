#include <stdio.h>
#include "config.h"
#include "motor_driver.h"

void motor_init(){
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MOTOR1);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, MOTOR2);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM2A, MOTOR3);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, MOTOR4);
    mcpwm_config_t pwm_config = {
        .frequency = 100, // 50Hz
        .cmpr_a = 0,     // duty cycle = 0
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_2, &pwm_config);
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);
}

void set_motor_1(int duty_perc){
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_perc);
}
void set_motor_2(int duty_perc){
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, duty_perc);
}
void set_motor_3(int duty_perc){
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A, duty_perc);
}
void set_motor_4(int duty_perc){
    mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, duty_perc);
}