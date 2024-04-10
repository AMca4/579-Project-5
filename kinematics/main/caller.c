#include <stdio.h>
#include "include/kinematics.h"

void app_main(void){
    float meters = 1;
    forward(meters);
    brake();
    reverse(meters);
    brake();
    fast_forward(meters);
}
