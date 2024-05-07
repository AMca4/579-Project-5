#include "include/ultrasonic.h"
#include "apds9960.h"
#include "vl53l1.h"
#include "i2c_top_level.h"
#include "webServer.h"
#include "kinematics.h"
#include <string.h>
#include "math.h"



#define LEFTSONIC_TRIGGER GPIO_NUM_16                                         // D16
#define LEFTSONIC_ECHO GPIO_NUM_17                                             // D17
#define RIGHTSONIC_TRIGGER GPIO_NUM_25                                        // D13
#define RIGHTSONIC_ECHO GPIO_NUM_26                                           // D14
#define FORWARD_MOTOR GPIO_NUM_5                                        // D32
#define REVERSE_MOTOR GPIO_NUM_18                                        // D33
#define LED GPIO_NUM_2                                                   // LED @ D2
#define IR_RIGHT GPIO_NUM_39                                             // D25
#define IR_LEFT GPIO_NUM_36                                              // D26                                                                                      // D0
#define SERVO GPIO_NUM_33                                                 // D5

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
float distanceCovered;

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
    float distToTarget; // Distance to Target, Invalid if straight ahead
};

void initialisations(){
    ultrasonic_initialisation(LEFTSONIC_TRIGGER, LEFTSONIC_ECHO);
    //ultrasonic_initialisation(RIGHTSONIC_TRIGGER, RIGHTSONIC_ECHO);
    i2c_master_initNew();
    wifi_connection();
    websocket_app_start();
    example_ledc_init();
    motor_innit();
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
    } 
    printf("\n Car waiting for Start Commandd");
    State = searchState;
    //int tofValue;
    //select_channel(TOF);
    //initialiseTOF();
    //tofValue = tofReading();
    //vTaskDelay(pdMS_TO_TICKS(1000));
    //turnStraight();
    //printf("Straight\n");
    //vTaskDelay(pdMS_TO_TICKS(1000));
    //turnLeft();
    //printf("Left\n");
    //vTaskDelay(pdMS_TO_TICKS(1000));
    //turnRight();
    //printf("Right\n");
    // turnStraight();
    // fast_forward(0.3);
    // brake();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // TESTING KINEMATICS
    turnStraight();
    forward(1);
    brake();
    vTaskDelay(pdMS_TO_TICKS(5000));
}


struct DetectionData searchStatefunc(){
    turnStraight();
    //fast_forward(5);
    //brake();
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("\nStart Searching\n");
    //forward_scan();
    struct DetectionData target;
    //if(readFlag() == false){State = waitState;}

    printf("\nReach Ultrasonic Scan");
    float wallReadingLeft = ultrasonic_scan(LEFTSONIC_TRIGGER, LEFTSONIC_ECHO);
    //float wallReadingRight = ultrasonic_scan(RIGHTSONIC_TRIGGER, RIGHTSONIC_ECHO);
    printf("Initial Wall Readings:");
    printf("%.2f", wallReadingLeft);
    printf(",   ");
    //printf("%.2f", wallReadingRight);
    //if(readFlag() == false){State = waitState;}

    // Send forward drive PWM signal to motor
    while(State == searchState){
        float searchReadingLeft = ultrasonic_scan(LEFTSONIC_TRIGGER, LEFTSONIC_ECHO);
        //float searchReadingRight = ultrasonic_scan(RIGHTSONIC_TRIGGER, RIGHTSONIC_ECHO);
        printf("\n Search Readings:");
        printf("%.2f", searchReadingLeft);
        printf(",   ");
        //printf("%.2f", searchReadingRight);
        float minDistLeft = 0.9*wallReadingLeft;
        //float minDistRight = 0.8*wallReadingRight;
        if (searchReadingLeft < minDistLeft){
            brake();
            printf("\n Detection Reading Left:");
            printf("%.2f", searchReadingLeft);
            target.direction = true; // True Indicates Left
            target.distToTarget = searchReadingLeft;  
            State = detectionState;         
        }// else if(searchReadingRight < minDistRight){ 
        //     brake();         
        //     printf("\n Detection Reading Right:");
        //     printf("%.2f", searchReadingRight);
        //     target.direction = false; // True Indicates Right
        //     target.distToTarget = searchReadingRight;  
        //     State = detectionState;
        //     //if(readFlag() == false){State = waitState;} 
        // }   
        //if(readFlag() == false){State = waitState;}
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    return target;

}

// ---------Detection State Function-------------------------------------

void detectionStatefunc(struct DetectionData *targetData){
    if (targetData->direction == true){
        printf("Reached Left Turn");
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("%f", targetData->distToTarget);
        if((targetData->distToTarget) > 0.35){
            printf("Reaching Condition");
            reverse((targetData->distToTarget) );
            brake();
            turnLeft();
            vTaskDelay(pdMS_TO_TICKS(100));
            float arc_length=M_PI*0.3/2;
            forward(arc_length);
            brake();
            turnStraight();
            // select_channel(TOF);
            // initialiseTOF();
            // float distToTarget = tofReading()/1000;
            
            int tofValue;
            select_channel(TOF);
            printf("\nDebug Reached Larger ToF initialisation");
            initialiseTOF();
            float tofToTarget = tofReading()/1000;
            forward(tofToTarget-0.05);
            printf("\nDebug Reached Larger");
            State = objIDState;
        } 
        else if (((targetData->distToTarget)<0.35) &&((targetData->distToTarget)>0.025)){
            reverse((targetData->distToTarget));
            brake();
            turnLeft();
            vTaskDelay(pdMS_TO_TICKS(100));
            float hypotenuse = targetData->distToTarget/0.707;
            printf("\nReaching Forward Hyptenuse");
            forward(hypotenuse - 0.05);
            printf("\nStuck Forward Hyptenuse");
            brake();
            turnStraight();
            printf("Reached TOF Reading");
            vTaskDelay(pdMS_TO_TICKS(2000));
            // select_channel(TOF);
            // initialiseTOF();
            // float distToTarget = tofReading()/1000;
            // forward(distToTarget-0.05);
            // float distToTarget;
            int tofValue;
            select_channel(TOF);
            //initialiseTOF();
            printf("\nDebug Reached Shorter\n");
            tofValue = tofReading();
            float tofToTarget = (float)tofValue/1000;
            printf("\nREaching HERE");
            printf("\n%.4f %d\n", tofToTarget,tofValue);
            forward((tofToTarget-0.05));
            brake();
            printf("\nDebug Reached Shorter");
            State = objIDState;
        } 

        //turnStraight();
    }
    else if (targetData->direction == false){
        if((targetData->distToTarget) > 0.35){
            reverse((targetData->distToTarget));
            brake();
            turnRight();
            vTaskDelay(pdMS_TO_TICKS(100));
            float arc_length=M_PI*0.3/2;
            forward(arc_length);
            brake();
            turnStraight();
            select_channel(TOF);
            initialiseTOF();
            float tofToTarget = tofReading()/1000;
            forward((tofToTarget-0.05));
            State = objIDState;
        } 
        else if (((targetData->distToTarget)<0.35) &&((targetData->distToTarget)>0.025)){
            reverse(targetData->distToTarget);
            brake();
            turnRight();
            vTaskDelay(pdMS_TO_TICKS(100));
            float hypotenuse = targetData->distToTarget/0.707;
            forward(hypotenuse - 0.05);
            brake();
            turnStraight();
            select_channel(TOF);
            initialiseTOF();
            float tofToTarget = tofReading()/1000;
            forward(tofToTarget-0.05);
            State = objIDState;
        } 
    }

    
    // if (ranFlag==0){select_channel(TOF);initialiseTOF();}
    // else {ranFlag = 1;}
    // // Advance distance to target function using target.distToTarget
    // float distToTarget = 0;
    // select_channel(TOF);
    // // initialiseTOF();
    // float distToTarget = tofReading();
    // //forward(distToTarget/1000); // Advance forward distance from tof, convert to meters
    // printf("\nFrom ToF, Advance to Target:");
    // printf("%.2f", distToTarget);
    //State = objIDState;
    //if(readFlag() == false){State = waitState;}

    // return distToTarget;

    


}

// ----------------Object Identification State Function-----------------

void objIDStatefunc(){

    // Read data from Infrared/colour sensor to determine colour
    // Advance and knock over pin if black/do nothing if white
    // Switch to returnState 
    // vTaskDelay(pdMS_TO_TICKS(5000));
    select_channel(LEFT_COLOUR);
    bool leftReading = colourReading();
    select_channel(RIGHT_COLOUR);
    bool rightReading = colourReading();

    if ((leftReading == 1) && (rightReading == 1)){
        //forward(0.5);
        printf("\nSkittle is Black from colour reading so call func to knock over");
        //reverse(0.5);
    } else {
        printf("\nSkittle is White so do nothing");
    }

    State = returnState;
    //if(readFlag() == false){State = waitState;}
}

// ----------------Return Car to Path State Function-------------------

void returnStatefunc(struct DetectionData *Data, float revDistance){

    // Maneouvre back to intial path (Left or right reversal)
    //reverse(revDistance/1000);
    // Switch to searchState

    if (Data->direction == false){
        //turnBackRight90();
        printf("\nPerform Right Reversal Maneouvre");

    } else {
        //turnBackLeft90();
        printf("\nPerform Left Reversal Maneouvre");

    }

    printf("Return to Initial State");
    State = searchState;
    //if(readFlag() == false){State = waitState;}
}


// -------------------Main App Loop----------------------

void main_app(void *pvParameters){
    initialisations();
    bool startFlag;
    // Call initial Motion forward kinematics forward func
    printf("Starting Program: \n");
    State = waitState;
    
    while(true){
        //if(readFlag() == false){State = waitState;}
        switch (State)
        {
        case waitState:
            waitStatefunc();
            
            break;
        case searchState:
            struct DetectionData data = searchStatefunc();
            break;       
        case detectionState:
            // distanceCovered = detectionStatefunc(&data);
            detectionStatefunc(&data);
            //forward((distanceCovered/1000)-0.15);
            //brake();
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
    xTaskCreate(main_app, "components_measurement", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);}
                                                                    // create a task for freeRTOS to manage,
                                                                    // identifier name "ultrasonic_scan", 
                                                                    // stack size=3*16kB
                                                                    // no pvParameters
                                                                    // priority=5 
                                                                    // task handle=null