
#include "include/ultrasonic.h"


#define LEFT_TRIGGER GPIO_NUM_5                                          // D5
#define LEFT_ECHO GPIO_NUM_18                                            // D18
#define RIGHT_TRIGGER GPIO_NUM_26                                        // D26
#define RIGHT_ECHO GPIO_NUM_27                                           // D27
#define MOTOR GPIO_NUM_4                                                 // D4

float distance_left, distance_right;

struct SensorData{
    float us_distance_left, us_distance_right;
};

struct SensorData get_measurements(){
    struct SensorData data;
    data.us_distance_left = ultrasonic_scan(LEFT_TRIGGER, LEFT_ECHO);
    data.us_distance_right = ultrasonic_scan(RIGHT_TRIGGER, RIGHT_ECHO);
    return data;
};

void initialisations(){
    // gpio_set_direction(MOTOR,GPIO_MODE_OUTPUT);
    // gpio_set_level(MOTOR,0); 
    ultrasonic_initialisation(LEFT_TRIGGER, LEFT_ECHO);
    ultrasonic_initialisation(RIGHT_TRIGGER, RIGHT_ECHO);
}

// void kinematics_innit_bruv(){

// }

void main_app(void *pvParameters){
    initialisations();
    while(true){
        struct SensorData data = get_measurements();
        printf("dist left: %0.04f cm\n", data.us_distance_left*100);                  // display the result to terminal 
        printf("dist right: %0.04f cm\n", data.us_distance_right*100);                  // display the result to terminal 
        vTaskDelay(pdMS_TO_TICKS(1000));                            // repeat every 1000ms=1s
    }
}

// void some_decision_algo_name(){
//     initialisations();
//     kinematics_innit_bruv();
// }

void app_main(){
    xTaskCreate(main_app, "components_measurement", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);}
                                                                    // create a task for freeRTOS to manage,
                                                                    // identifier name "ultrasonic_scan", 
                                                                    // stack size=3*16kB
                                                                    // no pvParameters
                                                                    // priority=5 
                                                                    // task handle=null