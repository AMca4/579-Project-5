#include "include/ultrasonic.h"
#include "apds9960.h"
#include "vl53l1.h"
#include "i2c_master.h"
#include <string.h>


#define LEFT_TRIGGER GPIO_NUM_5                                          // D5
#define LEFT_ECHO GPIO_NUM_20                                            // D18
#define RIGHT_TRIGGER GPIO_NUM_26                                        // D26
#define RIGHT_ECHO GPIO_NUM_27                                           // D27
#define MOTOR GPIO_NUM_4                                                 // D4
#define LED GPIO_NUM_2                                                   // LED @ D2

#define searchState 0 // Driving forward looking for obj
#define detectionState 1 // Detected object and manoeuvre to the object
#define objIDState 2 // Use infrareds to determine colour and either knock/avoid
#define returnState 3 // Return to main path


char State;


struct UltrasonicData{
    uint16_t leftDist;
    uint16_t rightDist;
};

struct APDSData{
    bool prox;
    int red;
    int green;
    int blue;
};

struct TOFData{
    int forwardDistance;
};

struct DetectionData{
    char direction; // Left, Right, Ahead
    uint16_t distToTarget; // Distance to Target, Invalid if straight ahead
};

void initialisations(){
    // gpio_set_direction(MOTOR,GPIO_MODE_OUTPUT);
    // gpio_set_level(MOTOR,0); 
    ultrasonic_initialisation(LEFT_TRIGGER, LEFT_ECHO);
    //ultrasonic_initialisation(RIGHT_TRIGGER, RIGHT_ECHO);
    i2c_master_initNew();
    //adxl345_init();
    initialiseTOF();
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

//--------------------------- STATE FUNCTIONS-------------------------------------//

// --------------------------Search State Function----------------------------------

struct DetectionData searchStatefunc(){
    // Get reading to wall and store
    // Align wall function
    // Drive Forward
    // Gather Data from Sensors until Detection is made
    // Switch State to detection state

    printf("Entered First State: \n");

    struct UltrasonicData initialReading;
    struct UltrasonicData searchReading;
    struct DetectionData target;

    initialReading.leftDist = ultrasonic_scan(LEFT_TRIGGER, LEFT_ECHO);
    //initialReading.rightDist = ultrasonic_scan(RIGHT_TRIGGER,RIGHT_ECHO);
    printf("Initial Wall Reading:");
    printf("%d", initialReading.leftDist);


    // Send forward drive PWM signal to motor
    while(State == searchState){
        
        searchReading.leftDist = ultrasonic_scan(LEFT_TRIGGER, LEFT_ECHO);
        //searchReading.rightDist = ultrasonic_scan(RIGHT_TRIGGER, RIGHT_ECHO);
        printf("\n Detection Reading:");
        printf("%u", searchReading.leftDist);
        float minErrorReadingDist = 10;
        float minDistLeft = 0.8*initialReading.leftDist;
        if ((searchReading.leftDist < minDistLeft) & (searchReading.leftDist > minErrorReadingDist)){
            // Stop Car Function
                int validateTarget[10] = {0};
                int valCount = 0;
                for(int i=0;i<10;i++){
                    
                    float valDist = ultrasonic_scan(LEFT_TRIGGER,LEFT_ECHO);
                    validateTarget[i] = valDist;
                }

                for(int i =0;i<10;i++){
                    
                    if((searchReading.leftDist < minDistLeft)&(searchReading.leftDist > minErrorReadingDist)){
                        
                        valCount++;

                    }
                    
                }

                if (valCount == 10){
                    printf("Target Detected, at Distance:");
                    printf("%u", searchReading.leftDist);
                    target.direction = true;
                    target.distToTarget = searchReading.leftDist;
                    State = detectionState;
                }

        }
        //else if (searchReading.rightDist < wallReading.rightDist) {
        //    // Stop Car Function
        //    target.direction = "Right";
        //    target.distToTarget = searchReading.rightDist;
        //    State = detectionState;
        //}
    }

    // Add in sensor detection at front of the car incase of object ahead

    return target;

}

// ---------Detection State Function-------------------------------------

void detectionStatefunc(struct DetectionData *targetData){
    
    struct TOFData fwDist;
    // Turn Left or Right based on Detection Data
    // After Turn Advance distance to target
    // Switch to objIDState
    if (targetData->direction == true){

        printf("\n Car Turns Left \n");
    }

    else if (targetData->direction == false){
        printf("\n Car Turns Right \n");
    }

    // Advance distance to target function using target.distToTarget

    int distToTarget;
    distToTarget = tofReading();
    
    fwDist.forwardDistance = distToTarget;

    printf("Advance to Target From Tof Reading:");
    printf("%d", distToTarget);


    State = objIDState;

}

// ----------------Object Identification State Function-----------------

void objIDStatefunc(){

    // Read data from Infrared/colour sensor to determine colour
    // Advance and knock over pin if black/do nothing if white
    // Switch to returnState 
    vTaskDelay(pdMS_TO_TICKS(5000));
    //bool validTarget = colourReading();
    bool validTarget = 1;

    if(validTarget == 1){
        printf("\nSkittle is Black from colour reading so call func to knock over");
    } else {
        printf("\nSkittle is White so do nothing");
    }

    State = returnState;

}

// ----------------Return Car to Path State Function-------------------

void returnStatefunc(struct DetectionData *Data){

    // Maneouvre back to intial path (Left or right reversal)
    // Switch to searchState

    if (Data->direction == false){

        printf("Perform Right Reversal Maneouvre");

    } else {

        printf("Perform Left Reversal Maneouvre");

    }


    //State = searchState;
}


// -------------------Main App Loop----------------------

void main_app(void *pvParameters){
    initialisations();
    
    // Call initial Motion forward kinematics forward func
    printf("Starting Program: \n");
    State = searchState;
    
    while(true){
        switch (State)
        {
        case searchState:
            struct DetectionData data = searchStatefunc();
            break;       
        case detectionState:
            detectionStatefunc(&data);
            break;
        case objIDState:
            objIDStatefunc();
            break;
        case returnState:
            returnStatefunc(&data);
            break;
        }
    }
}



void app_main(){
    xTaskCreate(main_app, "components_measurement", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);}
                                                                    // create a task for freeRTOS to manage,
                                                                    // identifier name "ultrasonic_scan", 
                                                                    // stack size=3*16kB
                                                                    // no pvParameters
                                                                    // priority=5 
                                                                    // task handle=null