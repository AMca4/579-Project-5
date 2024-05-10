#include <stdio.h>
#include "driver/gpio.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define IR_SENSOR_GPIO  5 // GPIO pin connected to the IR sensor

void app_main() {
    // Configure IR Sensor GPIO
    esp_rom_gpio_pad_select_gpio(IR_SENSOR_GPIO);
    gpio_set_direction(IR_SENSOR_GPIO, GPIO_MODE_INPUT);
    while (1) {
        // Read sensor value
        int sensor_value = gpio_get_level(IR_SENSOR_GPIO);

        // If sensor value is low, it indicates black is detected, else white is detected
        if (sensor_value == 0) {
            printf("Black detected!\n");
        } else {
            printf("White detected!\n");
        }

        // Delay for stability
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
