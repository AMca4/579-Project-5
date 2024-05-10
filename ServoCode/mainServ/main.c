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
#include "kinematics.h"
#include "webServer.h"


void app_main(void)
{       
        vTaskDelay(pdMS_TO_TICKS(2000));
        //wifi_connection();
        //websocket_app_start();
        int x = 1;
        while(true){
                //if(readFlag() && x <2){
                if( x <3){
                        motor_innit();
                        example_ledc_init(); //Call PWM Function  
                        turnStraight();
                        //printf("\nStraight\n");
                        vTaskDelay(pdMS_TO_TICKS(5000));
                        turnLeft();
                        //printf("\nLeft\n");
                        vTaskDelay(pdMS_TO_TICKS(5000));
                        //forward(0.2);
                        //brake();
                        //vTaskDelay(pdMS_TO_TICKS(2000));
                        //reverse(0.25);
                        //brake();
                        turnRight();
                        //printf("Right\n");
                        //vTaskDelay(pdMS_TO_TICKS(2000));
                        //forward(0.225);
                        //brake();
                        // vTaskDelay(pdMS_TO_TICKS(2000));
                        //reverse(0.225);
                        //brake();
                        //turnStraight();
                        //printf("Straight\n");
                        vTaskDelay(pdMS_TO_TICKS(2000));
                        //fast_forward(1);
                        //brake();
                        x = 2;
                }

                vTaskDelay(pdMS_TO_TICKS(1000));
        }
        motor_innit();
        forward_scan();
        int y=1;
        while(y<15){
                vTaskDelay(pdMS_TO_TICKS(100));
                y++;
        }
        brake();
        
}

