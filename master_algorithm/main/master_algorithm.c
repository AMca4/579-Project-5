#include "include/ultrasonic.h"
#include "include/accelerometer.h"


#define LEFT_TRIGGER GPIO_NUM_5                                          // D5
#define LEFT_ECHO GPIO_NUM_18                                            // D18
#define RIGHT_TRIGGER GPIO_NUM_26                                        // D26
#define RIGHT_ECHO GPIO_NUM_27                                           // D27
#define MOTOR GPIO_NUM_4                                                 // D4
#define LED GPIO_NUM_2                                                   // LED @ D2


float distance_left, distance_right;

struct SensorData{
    float us_distance_left, us_distance_right;
    int16_t x1, y1, x2, y2;
};

struct SensorData get_measurements(){
    struct SensorData data;
    data.us_distance_left = ultrasonic_scan(LEFT_TRIGGER, LEFT_ECHO);
    data.us_distance_right = ultrasonic_scan(RIGHT_TRIGGER, RIGHT_ECHO);
    // int16_t x1, y1, x2, y2;
    // adxl345_read_acceleration(&x1, &y1);
    // vTaskDelay(pdMS_TO_TICKS(150)); 
    // adxl345_read_acceleration(&x2, &y2);
    // data.x1 = x1;
    // data.x2 = x2;
    // data.y1 = y1;
    // data.y2 = y2;
    return data;
};

void initialisations(){
    // gpio_set_direction(MOTOR,GPIO_MODE_OUTPUT);
    // gpio_set_level(MOTOR,0); 
    ultrasonic_initialisation(LEFT_TRIGGER, LEFT_ECHO);
    ultrasonic_initialisation(RIGHT_TRIGGER, RIGHT_ECHO);
    // i2c_master_init();
    // adxl345_init();
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

// void kinematics_innit_bruv(){

// }

void main_app(void *pvParameters){
    initialisations();
    while(true){
        struct SensorData data = get_measurements();
        printf("left: %0.04f cm right: %0.04f cm\n", (data.us_distance_right), (data.us_distance_left));                  
        // ////////// ACCELEROMETER ///////////
        // if (((data.x2-data.x1) > 30) | ((data.x1-data.x2) > 30 )) {
        //     gpio_set_level(LED, 1);
        //     vTaskDelay(pdMS_TO_TICKS(200));
        //     gpio_set_level(LED, 0);
        // }
        // else if(((data.y2-data.y1) > 30) | ((data.y1-data.y2) > 30)){
        //     gpio_set_level(LED, 1);
        //     vTaskDelay(pdMS_TO_TICKS(50));
        //     gpio_set_level(LED, 0);
        //     vTaskDelay(pdMS_TO_TICKS(50));
        //     gpio_set_level(LED, 1);
        //     vTaskDelay(pdMS_TO_TICKS(50));
        //     gpio_set_level(LED, 0);
        // }
        // printf("X: %d, Y: %d\n", data.x2, data.y2);
        // vTaskDelay(pdMS_TO_TICKS(100));
        // //////////END OF ACCELEROMETER ///////////
                         
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