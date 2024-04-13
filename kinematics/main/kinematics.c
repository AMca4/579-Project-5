#include "include/kinematics.h"

#define forward_pin 33
#define reverse_pin 32
#define tyre_circumference 0.0895354

void motor_innit(){
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, forward_pin);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, reverse_pin);
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 40000;    // Set frequency in Hz
    pwm_config.cmpr_a = 0;           // Initial duty cycle of 0%
    pwm_config.cmpr_b = 0;           // Initial duty cycle of 0%
    pwm_config.counter_mode = MCPWM_UP_DOWN_COUNTER;
    // pwm_config.duty_mode =  MCPWM_DUTY_MODE_0;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_1;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
}

void forward(float meters) {
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 60);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    float duration = meters*100 / tyre_circumference;
    vTaskDelay(pdMS_TO_TICKS(duration));
    
}

void fast_forward(float meters){
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 70);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    float duration = meters*100 / tyre_circumference;
    vTaskDelay(pdMS_TO_TICKS(duration));
}

void reverse(float meters){
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 60);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    float duration = meters*100 / tyre_circumference;
    vTaskDelay(pdMS_TO_TICKS(duration));
}

void brake(){
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
}