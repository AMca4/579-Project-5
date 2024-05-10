#include "include/ultrasonic.h"
#include <portmacro.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
                                      
#define ROUNDTRIP 5822.1f                                           // room temp speed of sound at sea level (343/2*e6)^-1=5822.08cm/s
#define PING_TIMEOUT 6000                                           // min time after pulse is considered lost
#define timeout_expired(start, len) ((esp_timer_get_time() - (start)) >= (len))

#define TRIGGER_GPIO GPIO_NUM_12
#define ECHO_GPIO GPIO_NUM_13

#define MAX_DISTANCE_CM 400
#define TIMEOUT_US 20000


esp_err_t ultrasonic_initialisation(gpio_num_t TRIGGER, gpio_num_t ECHO){
    gpio_set_direction(TRIGGER, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO, GPIO_MODE_INPUT);
    return gpio_set_level(TRIGGER, 0);
}

uint64_t ultrasonic_scan(gpio_num_t TRIGGER, gpio_num_t ECHO){

    gpio_set_level(TRIGGER_GPIO, 1);
    ets_delay_us(10);
    gpio_set_level(TRIGGER_GPIO, 0);

    // Wait for the ECHO pin to go high
    while (!gpio_get_level(ECHO_GPIO)) {}

    // Start measuring time
    uint64_t start_time = esp_timer_get_time();

    // Wait for the ECHO pin to go low
    while (gpio_get_level(ECHO_GPIO)) {
        if ((esp_timer_get_time() - start_time) > TIMEOUT_US) {
            return MAX_DISTANCE_CM;
        }
    }

    // Calculate duration of echo pulse
    uint64_t duration = esp_timer_get_time() - start_time;

    // Convert duration to distance in centimeters
    return duration * 0.034 / 2;

}