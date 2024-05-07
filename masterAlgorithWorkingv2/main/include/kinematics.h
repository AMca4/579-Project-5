#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "driver/ledc.h"



// struct pwm_details{
//     int32_t frequency;
//     float duty_cycle;
//     gpio_num_t pin;
//     float duration;
// };

void motor_innit();

void forward(float meters);

void forward_scan();

void cheeky_steer();

void fast_forward(float meters);

void reverse(float meters);

void brake();

void turnRight();

void turnForwardRight90();

void turnBackRight90();

void turnLeft();

void turnForwardLeft90();

void turnBackLeft90();

void turnStraight();

void example_ledc_init(void);

#endif
#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "driver/ledc.h"



// struct pwm_details{
//     int32_t frequency;
//     float duty_cycle;
//     gpio_num_t pin;
//     float duration;
// };

void motor_innit();

void forward(float meters);

void fast_forward(float meters);

void reverse(float meters);

void brake();

void turnRight();

void turnLeft();

void turnStraight();

void example_ledc_init(void);

#endif