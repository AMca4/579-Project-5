#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
// #include "driver/mcpwm.h"
// #include "soc/mcpwm_reg.h"
// #include "soc/mcpwm_struct.h"
#include "driver/ledc.h"
#include "include/ultrasonic.h"
#include "include/i2c_top_level.h"
#include "include/kinematics.h"
#include "include/webServer.h"

#define LEFT_TRIGGER GPIO_NUM_16
#define LEFT_ECHO GPIO_NUM_17
#define RIGHT_TRIGGER GPIO_NUM_25
#define RIGHT_ECHO GPIO_NUM_26

void initialisations(){
    // gpio_set_direction(MOTOR,GPIO_MODE_OUTPUT);
    // gpio_set_level(MOTOR,0); 
    ultrasonic_initialisation(LEFT_TRIGGER, LEFT_ECHO);
    ultrasonic_initialisation(RIGHT_TRIGGER, RIGHT_ECHO);
    i2c_master_initNew();
    // wifi_connection();
    // websocket_app_start();
    example_ledc_init();
    //adxl345_init();
    //gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void app_main(void){
    initialisations();
    while(1){
        float left = ultrasonic_scan(LEFT_TRIGGER, LEFT_ECHO);
        float right = ultrasonic_scan(RIGHT_TRIGGER, RIGHT_ECHO);
        printf("left %f, right %f\n", left, right);
    }
}
