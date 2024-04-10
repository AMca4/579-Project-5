#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

void forward(float meters);

void fast_forward(float meters);

void reverse(float meters);

void brake();

#endif