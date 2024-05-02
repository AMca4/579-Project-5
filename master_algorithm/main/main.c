#include <string.h>
#include "include/i2c_top_level.h"
#include "include/ultrasonic.h"
#include "include/apds9960.h"
#include "include/vl53l1.h"
#include "include/webServer.h"
#include "include/kinematics.h"
// #include "accelerometer.h"


#define LEFTSONIC_TRIGGER GPIO_NUM_16                                         // D16
#define LEFTSONIC_ECHO GPIO_NUM_17                                             // D17
#define RIGHTSONIC_TRIGGER GPIO_NUM_13                                        // D13
#define RIGHTSONIC_ECHO GPIO_NUM_14                                           // D14
#define FORWARD_MOTOR GPIO_NUM_32                                        // D32
#define REVERSE_MOTOR GPIO_NUM_33                                        // D33
#define LED GPIO_NUM_2                                                   // LED @ D2
#define IR_RIGHT GPIO_NUM_25                                             // D25
#define IR_LEFT GPIO_NUM_26                                              // D26                                                                                      // D0
#define SERVO GPIO_NUM_5                                                 // D5

#define TOF  2
#define AXL  0
#define LEFT_COLOUR  1
#define RIGHT_COLOUR  3


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
    ultrasonic_initialisation(LEFTSONIC_TRIGGER, LEFTSONIC_ECHO);
    ultrasonic_initialisation(RIGHTSONIC_TRIGGER, RIGHTSONIC_ECHO);
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
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    State = searchState;

}


struct DetectionData searchStatefunc(){
    // Get reading to wall and store
    // Align wall function
    // Drive Forward
    // Gather Data from Sensors until Detection is made
    // Switch State to detection state

    printf("\nStart Searching\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
    struct DetectionData target;
    vTaskDelay(pdMS_TO_TICKS(1000));
    if(readFlag() == false){State = waitState;}
    float wallReadingLeft = ultrasonic_scan(LEFTSONIC_TRIGGER, LEFTSONIC_ECHO);
    float wallReadingRight = ultrasonic_scan(RIGHTSONIC_TRIGGER, RIGHTSONIC_ECHO);
    printf("Initial Wall Readings:");
    printf("%.2f", wallReadingLeft);
    printf(",   ");
    printf("%.2f", wallReadingRight);
    if(readFlag() == false){State = waitState;}

    // Send forward drive PWM signal to motor
    while(State == searchState){
        
        float searchReadingLeft = ultrasonic_scan(LEFTSONIC_TRIGGER, LEFTSONIC_TRIGGER);
        float searchReadingRight = ultrasonic_scan(RIGHTSONIC_TRIGGER, RIGHTSONIC_ECHO);
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
            if(readFlag() == false){State = waitState;} 
        }   
        if(readFlag() == false){State = waitState;}
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    return target;

}

// ---------Detection State Function-------------------------------------

float detectionStatefunc(struct DetectionData *targetData){
    
    // Turn Left or Right based on Detection Data
    // After Turn Advance distance to target
    // Switch to objIDState
    if (targetData->direction == true){
        turnLeft();
        forward(0.25);
        printf("\n Car Turns Left \n");
    }

    else if (targetData->direction == false){
        turnRight();
        forward(0.25);
        printf("\n Car Turns Right \n");
    }

    
    if (ranFlag==0){initialiseTOF();}
    else {ranFlag = 1;}
    // Advance distance to target function using target.distToTarget
    select_channel(TOF);
    int distToTarget = tofReading();
    forward(distToTarget*1000); // Advance forward distance from tof, convert to meters
    printf("\nFrom ToF, Advance to Target:");
    printf("%d", distToTarget);
    State = objIDState;
    if(readFlag() == false){State = waitState;}
    float distanceCovered = distToTarget - 50;
    return distanceCovered;
}

// ----------------Object Identification State Function-----------------

void objIDStatefunc(){

    // Read data from Infrared/colour sensor to determine colour
    // Advance and knock over pin if black/do nothing if white
    // Switch to returnState 
    vTaskDelay(pdMS_TO_TICKS(5000));
    select_channel(LEFT_COLOUR);
    bool validTargetA = colourReading();
    select_channel(RIGHT_COLOUR);
    bool validTargetB = colourReading();

    if((validTargetA == 1) & (validTargetB == 1)){
        printf("\nSkittle is Black from colour reading so call func to knock over");
        forward(0.1);
        reverse(0.1);
    } else {
        printf("\nSkittle is White so do nothing");
    }

    State = returnState;
    if(readFlag() == false){State = waitState;}
}

// ----------------Return Car to Path State Function-------------------

void returnStatefunc(struct DetectionData *Data, float revDistance){
    // Maneouvre back to intial path (Left or right reversal)
    reverse(revDistance/1000);
    // Switch to searchState

    if (Data->direction == false){
        turnRight();
        reverse(0.25);
        printf("\nPerform Right Reversal Maneouvre");
        turnStraight();

    } else {
        turnLeft();
        reverse(0.25);
        printf("\nPerform Left Reversal Maneouvre");
        turnStraight();

    }

    printf("Return to Initial State");
    State = searchState;
    if(readFlag() == false){State = waitState;}
}


// -------------------Main App Loop----------------------

void main_app(){
    initialisations();
    bool startFlag;
    // Call initial Motion forward kinematics forward func
    printf("Starting Program: \n");
    State = waitState;
    
    while(true){
        if(readFlag() == false){State = waitState;}
        float distanceCovered=0;
        switch (State)
        {
        case waitState:
            waitStatefunc();
            
            break;
        case searchState:
            struct DetectionData data = searchStatefunc();
            
            break;       
        case detectionState:
            distanceCovered = detectionStatefunc(&data);
            
            break;
        case objIDState:
            objIDStatefunc();
            
            break;
        case returnState:
            returnStatefunc(&data, distanceCovered);
            break;
        }
    }
}



void app_main(){
    xTaskCreate(main_app, "components_measurement", configMINIMAL_STACK_SIZE * 10, NULL, 5, NULL);}
                                                                    // create a task for freeRTOS to manage,
                                                                    // identifier name "ultrasonic_scan", 
                                                                    // stack size=3*16kB
                                                                    // no pvParameters
                                                                    // priority=5 
                                                                    // task handle=null