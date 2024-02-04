/* LEDC (LED Controller) PWM Generation
PWM Code to determine the position of a micro servo motor (SG90)
Written by yours truly Lewis Anderson
*/
#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define LEDC_TIMER              LEDC_TIMER_0 // Timer 0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE // Set to Low Speed mode
#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0 // LEDC Channel 0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (50) // Frequency in Hertz. Set frequency at 50Hz, T = 20ms period

static void example_ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,       // Low speed mode
        .duty_resolution  = LEDC_DUTY_RES,   // 13 Bit resolution (8192 count)
        .timer_num        = LEDC_TIMER,      // Timer 0
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK    // Set to Auto clk
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,         // Low Speed mode
        .channel        = LEDC_CHANNEL,      // Channel 0
        .timer_sel      = LEDC_TIMER,        // Timer 0
        .intr_type      = LEDC_INTR_DISABLE, // Disable Interrupts 
        .gpio_num       = LEDC_OUTPUT_IO,    // GPIO D5
        .duty           = 0,                 // Initialise duty to 0% (LED OFF)
        .hpoint         = 0                  // Highest Point ((2^duty_resolution) - 1)           
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

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
            printf("Hello World I am Lewis \n");
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}