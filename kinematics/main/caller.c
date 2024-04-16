#include <stdio.h>
#include "include/kinematics.h"

void app_main(void){
    float meters = 1;
    motor_innit();
    // forward(meters);
    // brake();
    // reverse(meters);
    // brake();
    vTaskDelay(pdMS_TO_TICKS(2000));
    fast_forward(meters);
    brake();
}


