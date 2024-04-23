#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "servo.h"
#include "sdkconfig.h"

void app_main(void)
{
    example_ledc_init(); //Call PWM Function  
    while(1){ //Infinite Loop

        uint16_t i;
        uint16_t dutyC = 408; // (Position 0)
        for(i=0;i<40;i++)    // From Positon 0 to 180
		{
            dutyC = dutyC + 10; // Increment duty cycle size (0 -> 180 deg)
            //---Set and update duty cycle---//
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, dutyC)); // Set Duty Cycle to incrementing value(LED ON)
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));     // Update Duty Cycle
            //-------------------------------//
        
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}