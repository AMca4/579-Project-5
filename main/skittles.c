#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp32/rom/ets_sys.h"

#define TRIG GPIO_NUM_5
#define ECHO GPIO_NUM_18

float distance;

void ultrasonic_initialisation(void){
    gpio_set_direction(TRIG,GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ECHO, GPIO_PULLUP_ONLY);
    gpio_set_level(TRIG, 0);
}

float ultrasonic(void){
    ets_delay_us(4); // we want 4us
    gpio_set_level(TRIG, 1);
    ets_delay_us(10); // we want 10us
    gpio_set_level(TRIG, 0);
    int64_t echo_start = esp_timer_get_time();
    int64_t echo_stop = echo_start;
    while(gpio_get_level(ECHO)){
        echo_stop = esp_timer_get_time();
    }
    distance = (echo_stop - echo_start) *(1000000) /58.2;
    return distance;
}
void app_main(void){
    ultrasonic_initialisation();
    while (1){
        distance = ultrasonic();
        printf("The distance is: %f cm \n", distance);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

}
