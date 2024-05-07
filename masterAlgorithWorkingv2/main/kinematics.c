
#include "include/kinematics.h"

#define forward_pin 33
#define reverse_pin 18
#define tyre_circumference 0.0895354

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (5)
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT
#define LEDC_DUTY               (4096)
#define LEDC_FREQUENCY          (50)

// void example_ledc_init(void);

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
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 75);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    float duration = meters*100 / (tyre_circumference*0.794);
    vTaskDelay(pdMS_TO_TICKS(duration));
    
}

void fast_forward(float meters){
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 99);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    float duration = meters*100 / (tyre_circumference*0.88);
    vTaskDelay(pdMS_TO_TICKS(duration));
}

void reverse(float meters){
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 75);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    float duration = meters*100 / (tyre_circumference*0.794);
    vTaskDelay(pdMS_TO_TICKS(duration));
}

void brake(){
    mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
    mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);

}

void cheeky_steer(){
    uint16_t dutyC = 388; // (Position 0)
    //---Set and update duty cycle---//
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, dutyC)); // Set Duty Cycle to incrementing value(LED ON)
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));     // Update Duty Cycle
}

void turnLeft(){
        uint16_t dutyC = 600; // (Position 0)
        //---Set and update duty cycle---//
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, dutyC)); // Set Duty Cycle to incrementing value(LED ON)
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));     // Update Duty Cycle
}

void turnForwardLeft90(){
    turnLeft();
    vTaskDelay(pdMS_TO_TICKS(100));
    forward(1);
    brake();
    turnStraight();
}

void turnBackLeft90(){
    turnLeft();
    vTaskDelay(pdMS_TO_TICKS(100));
    reverse(0.95);
    brake();
    turnStraight();
}

void turnRight(){
        uint16_t dutyC = 250; // (Position 0)
        //---Set and update duty cycle---//
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, dutyC)); // Set Duty Cycle to incrementing value(LED ON)
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));     // Update Duty Cycle
}

void turnForwardRight90(){

    turnRight();
    vTaskDelay(pdMS_TO_TICKS(100));
    forward(0.99);
    brake();
    turnStraight();
}

void turnBackRight90(){

    turnRight();
    vTaskDelay(pdMS_TO_TICKS(100));
    reverse(0.95);
    brake();
    turnStraight();
}

void turnStraight(){
        uint16_t dutyC = 400; // (Position 0)
        //---Set and update duty cycle---//
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, dutyC)); // Set Duty Cycle to incrementing value(LED ON)
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));     // Update Duty Cycle
}

void forward_scan(){
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 70);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}


void example_ledc_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}