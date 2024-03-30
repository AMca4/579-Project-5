#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdio.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include "driver/gpio.h"
#include <esp_timer.h>
#include "esp32/rom/ets_sys.h"

float ultrasonic_scan(gpio_num_t TRIGGER, gpio_num_t ECHO);

esp_err_t ultrasonic_initialisation(gpio_num_t TRIGGER, gpio_num_t ECHO);

#endif