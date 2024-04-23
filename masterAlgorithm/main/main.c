#include "include/ultrasonic.h"
#include "apds9960.h"
#include "vl53l1.h"
#include "i2c_master.h"
#include "webServer.h"
#include <string.h>


#define LEFT_TRIGGER GPIO_NUM_5                                          // D5
#define LEFT_ECHO GPIO_NUM_6                                            // D18
#define RIGHT_TRIGGER GPIO_NUM_15                                        // D26
#define RIGHT_ECHO GPIO_NUM_16                                           // D27
#define MOTOR GPIO_NUM_4                                                 // D4
#define LED GPIO_NUM_2                                                   // LED @ D2

#define searchState 0 // Driving forward looking for obj
#define detectionState 1 // Detected object and manoeuvre to the object
#define objIDState 2 // Use infrareds to determine colour and either knock/avoid
#define returnState 3 // Return to main path
#define waitState 4 // Wait for start signal to be sent


char State;
bool ranFlag = 0;


struct UltrasonicData{
    float leftDist;
    float rightDist;
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
    bool direction; // Left, Right, Ahead
    uint16_t distToTarget; // Distance to Target, Invalid if straight ahead
};

void initialisations(){
    // gpio_set_direction(MOTOR,GPIO_MODE_OUTPUT);
    // gpio_set_level(MOTOR,0); 
    ultrasonic_initialisation(LEFT_TRIGGER, LEFT_ECHO);
    ultrasonic_initialisation(RIGHT_TRIGGER, RIGHT_ECHO);
    i2c_master_initNew();
    wifi_connection();
    websocket_app_start();
    //adxl345_init();
    //gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

//--------------------------- STATE FUNCTIONS-------------------------------------//

// --------------------------Search State Function----------------------------------

void waitStatefunc(){
    
    bool signal = readFlag();

    if(signal == true){
        printf("\nStart Message Received");
        State = searchState;
    } else if(signal == false){
        printf("\n Car waiting for Start Command");
    }

}


struct DetectionData searchStatefunc(){
    // Get reading to wall and store
    // Align wall function
    // Drive Forward
    // Gather Data from Sensors until Detection is made
    // Switch State to detection state

    printf("\nStart Searching\n");
    struct DetectionData target;
    float wallReadingLeft = ultrasonic_scan(LEFT_TRIGGER, LEFT_ECHO);
    float wallReadingRight = ultrasonic_scan(RIGHT_TRIGGER, RIGHT_ECHO);
    printf("Initial Wall Readings:");
    printf("%.2f", wallReadingLeft);
    printf(",   ");
    printf("%.2f", wallReadingRight);


    // Send forward drive PWM signal to motor
    while(State == searchState){
        
        float searchReadingLeft = ultrasonic_scan(LEFT_TRIGGER, LEFT_ECHO);
        float searchReadingRight = ultrasonic_scan(RIGHT_TRIGGER, RIGHT_ECHO);
        printf("\n Search Readings:");
        printf("%.2f", searchReadingLeft);
        printf(",   ");
        printf("%.2f", searchReadingRight);
        float minDistLeft = 0.6*wallReadingLeft;
        float minDistRight = 0.6*wallReadingRight;
        if (searchReadingLeft < minDistLeft){
            printf("\n Detection Reading Left:");
            printf("%.2f", searchReadingLeft);
            target.direction = true; // True Indicates Left
            target.distToTarget = searchReadingLeft;  
            State = detectionState;         
        } else if(searchReadingRight < minDistRight){          
            printf("\n Detection Reading Right:");
            printf("%.2f", searchReadingRight);
            target.direction = false; // True Indicates Right
            target.distToTarget = searchReadingRight;  
            State = detectionState; 
        }   
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    return target;

}

// ---------Detection State Function-------------------------------------

void detectionStatefunc(struct DetectionData *targetData){
    
    // Turn Left or Right based on Detection Data
    // After Turn Advance distance to target
    // Switch to objIDState
    if (targetData->direction == true){

        printf("\n Car Turns Left \n");
    }

    else if (targetData->direction == false){
        printf("\n Car Turns Right \n");
    }

    
    if (ranFlag==0){initialiseTOF();}
    else {ranFlag = 1;}
    // Advance distance to target function using target.distToTarget
    int distToTarget;
    distToTarget = tofReading();

    printf("\nFrom ToF, Advance to Target:");
    printf("%d", distToTarget);
    State = objIDState;

}

// ----------------Object Identification State Function-----------------

void objIDStatefunc(){

    // Read data from Infrared/colour sensor to determine colour
    // Advance and knock over pin if black/do nothing if white
    // Switch to returnState 
    vTaskDelay(pdMS_TO_TICKS(5000));
    bool validTarget = colourReading();

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

        printf("\nPerform Right Reversal Maneouvre");

    } else {

        printf("\nPerform Left Reversal Maneouvre");

    }

    printf("Return to Initial State");
    State = searchState;
}


// -------------------Main App Loop----------------------

void main_app(void *pvParameters){
    initialisations();
    
    // Call initial Motion forward kinematics forward func
    printf("Starting Program: \n");
    State = waitState;
    
    while(true){
        switch (State)
        {
        case waitState:
            waitStatefunc();
            break;
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