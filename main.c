#include "include/ultrasonic.h"
#include "include/accelerometer.h"


#define LEFT_TRIGGER GPIO_NUM_5                                          // D5
#define LEFT_ECHO GPIO_NUM_18                                            // D18
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
    float leftDist;
    float rightDist;
};

struct DetectionData{
    char direction; // Left, Right, Ahead
    float distToTarget; // Distance to Target, Invalid if straight ahead
};

void initialisations(){
    // gpio_set_direction(MOTOR,GPIO_MODE_OUTPUT);
    // gpio_set_level(MOTOR,0); 
    ultrasonic_initialisation(LEFT_TRIGGER, LEFT_ECHO);
    ultrasonic_initialisation(RIGHT_TRIGGER, RIGHT_ECHO);
    i2c_master_init();
    adxl345_init();
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

// -------------------Main App Loop----------------------

void main_app(void *pvParameters){
    initialisations();
    
    // Call initial Motion forward kinematics forward func

    State = searchState;
    
    while(true){

        switch (State)
        {
        case searchState:
            struct DetectionData target = searchStatefunc();
            break;       
        case detectionState:
            detectionStatefunc(target);
            break;
        case objIDState:
            objIDStatefunc();
            break;
        case returnState:
            returnStatefunc(target);
            break;
        }
    }
}

//--------------------------- STATE FUNCTIONS-------------------------------------//

// --------------------------Search State Function----------------------------------

struct DetectionData searchStatefunc(){
    // Get reading to wall and store
    // Align wall function
    // Drive Forward
    // Gather Data from Sensors until Detection is made
    // Switch State to detection state

    struct UltrasonicData wallReading;
    wallReading.leftDist = ultrasonic_scan(LEFT_TRIGGER, LEFT_ECHO);
    wallReading.rightDist = ultrasonic_scan(RIGHT_TRIGGER,RIGHT_ECHO);

    struct UltrasonicData searchReading;
    struct DetectionData target;

    // Send forward drive PWM signal to motor
    while(State == searchState){
        
        searchReading.leftDist = ultrasonic_scan(LEFT_TRIGGER, LEFT_ECHO);
        searchReading.rightDist = ultrasonic_scan(RIGHT_TRIGGER, RIGHT_ECHO);

        if (searchReading.leftDist < wallReading.leftDist){
            // Stop Car Function
            target.direction = "Left";
            target.distToTarget = searchReading.leftDist;
            State = detectionState;
        }
        else if (searchReading.rightDist < wallReading.rightDist) {
            // Stop Car Function
            target.direction = "Right";
            target.distToTarget = searchReading.rightDist;
            State = detectionState;
        }
    }

    // Add in sensor detection at front of the car incase of object ahead
    return target;

}

// ---------Detection State Function-------------------------------------

detectionStatefunc(struct DetectionData target){
    
    
    // Turn Left or Right based on Detection Data
    // After Turn Advance distance to target
    // Switch to objIDState
    if (target.direction = "Left"){

        // Turn Car left Function
    }

    else if (target.direction = "Right"){

        // Turn Car Right Function
    }

    // Advance distance to target function using target.distToTarget

    State = objIDState;

}

// ----------------Object Identification State Function-----------------

objIDStatefunc(){

    // Read data from Infrared/colour sensor to determine colour
    // Advance and knock over pin if black/do nothing if white
    // Switch to returnState 

    bool validTarget = infrared_reading();

    if(validTarget == 1){
        // Drive forward set distance to knock over skittle
    }

    State = returnState;

}

// ----------------Return Car to Path State Function-------------------

returnStatefunc(struct DetectionData target){

    // Maneouvre back to intial path (Left or right reversal)
    // Switch to searchState

    if (target.direction = "Right" ){

        // Perform Right Reversal Maneouvre

    } else {

        // Perform Left Reversal Maveouvre

    }


    State = searchState;
}




void app_main(){
    xTaskCreate(main_app, "components_measurement", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);}
                                                                    // create a task for freeRTOS to manage,
                                                                    // identifier name "ultrasonic_scan", 
                                                                    // stack size=3*16kB
                                                                    // no pvParameters
                                                                    // priority=5 
                                                                    // task handle=null