#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#include "include/kinematics.h"



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

#endif