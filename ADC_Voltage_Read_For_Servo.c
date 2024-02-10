// Receive analog input to control servo motor
// Lewis Anderson

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
//#include "esp_err.h"
//#include "esp_log.h"

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
    example_ledc_init(); //Call PWM Setup
    uint16_t dutyC;
    gpio_reset_pin(GPIO_NUM_34);
    gpio_set_direction(GPIO_NUM_34, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_34, GPIO_PULLDOWN_ONLY);

    unsigned voltage;
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12);
    adc1_ulp_enable();

    while (1)
    { // Infinite Loop
        voltage = adc1_get_raw(ADC1_CHANNEL_6);
        //printf("%u\n",voltage);
        //vTaskDelay(100 / portTICK_PERIOD_MS);

    if(voltage > 1000){
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 900)); // Set Duty Cycle to incrementing value(LED ON)
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));     // Update Duty Cycle
    }else{
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 400)); // Set Duty Cycle to incrementing value(LED ON)
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));     // Update Duty Cycle  
    }
    }
}