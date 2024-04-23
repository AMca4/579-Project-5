#include "include/ultrasonic.h"
#include <portmacro.h>

#define MAX_DISTANCE_CM 400                                         // 4m max
#define ROUNDTRIP 5822.1f                                           // room temp speed of sound at sea level (343/2*e6)^-1=5822.08cm/s
#define PING_TIMEOUT 6000                                           // min time after pulse is considered lost
#define timeout_expired(start, len) ((esp_timer_get_time() - (start)) >= (len))


esp_err_t ultrasonic_initialisation(gpio_num_t TRIGGER, gpio_num_t ECHO){
    gpio_set_direction(TRIGGER, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO, GPIO_MODE_INPUT);
    return gpio_set_level(TRIGGER, 0);
}

float ultrasonic_scan(gpio_num_t TRIGGER, gpio_num_t ECHO){
    static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    uint32_t max_time_taken= MAX_DISTANCE_CM*ROUNDTRIP;
    portENTER_CRITICAL(&mux);                                   // disable task scheduling, preventing other tasks from running concurrently
    gpio_set_level(TRIGGER, 0);                                 // make sure the trigger is off to mitigate any reflected pulses
    ets_delay_us(4);                                            // delay 4us before turning on
    gpio_set_level(TRIGGER, 1);                                 // make trigger active to send pulses
    ets_delay_us(10);                                           // delay 10us to allow the transducers to produce 8 pulses (40kHz)
    gpio_set_level(TRIGGER, 0);                                 // turn trigger off to stop sending pulses
    if (gpio_get_level(ECHO))                                   // Previous ping isn't ended
        portEXIT_CRITICAL(&mux);                                // exit to avoid mismatch
    int64_t start = esp_timer_get_time();                       // get time in microseconds from boot time
    while (!gpio_get_level(ECHO)){                              // Wait for echo
        if (timeout_expired(start, PING_TIMEOUT))               // check for timeout
            portEXIT_CRITICAL(&mux);                            // exit back to task sceduling due to ping time out
    }
    int64_t echo_rising_edge = esp_timer_get_time();            // get time in microseconds from boot time
    int64_t time = echo_rising_edge;                            // match time to echo_rising_edge (can only be >= to avoid getting negative numbers)
    while (gpio_get_level(ECHO)){                               // got echo, measuring
        time = esp_timer_get_time();
        if (timeout_expired(echo_rising_edge, max_time_taken))  // check for timeout
            portEXIT_CRITICAL(&mux);                            // exit back to task sceduling due to ping time out
    }
    portEXIT_CRITICAL(&mux);                                    // done, exiting back to task scheduling
    uint32_t time_taken = time - echo_rising_edge;              // calculate the time it took for the pulse to travel distance*2
    float distance = time_taken / ROUNDTRIP;                    // calculate distance (one way only)
    return distance;
}

float ultrasonic_scan_multireading(gpio_num_t TRIGGER, gpio_num_t ECHO){
    static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    uint32_t max_time_taken= MAX_DISTANCE_CM*ROUNDTRIP;
    float readings [5];
    for(int i = 0;i<5;i++){
        portENTER_CRITICAL(&mux);                                   // disable task scheduling, preventing other tasks from running concurrently
        gpio_set_level(TRIGGER, 0);                                 // make sure the trigger is off to mitigate any reflected pulses
        ets_delay_us(4);                                            // delay 4us before turning on
        gpio_set_level(TRIGGER, 1);                                 // make trigger active to send pulses
        ets_delay_us(10);                                           // delay 10us to allow the transducers to produce 8 pulses (40kHz)
        gpio_set_level(TRIGGER, 0);                                 // turn trigger off to stop sending pulses
        if (gpio_get_level(ECHO))                                   // Previous ping isn't ended
            portEXIT_CRITICAL(&mux);                                // exit to avoid mismatch
        int64_t start = esp_timer_get_time();                       // get time in microseconds from boot time
        while (!gpio_get_level(ECHO)){                              // Wait for echo
            if (timeout_expired(start, PING_TIMEOUT))               // check for timeout
                portEXIT_CRITICAL(&mux);                            // exit back to task sceduling due to ping time out
        }
        int64_t echo_rising_edge = esp_timer_get_time();            // get time in microseconds from boot time
        int64_t time = echo_rising_edge;                            // match time to echo_rising_edge (can only be >= to avoid getting negative numbers)
        while (gpio_get_level(ECHO)){                               // got echo, measuring
            time = esp_timer_get_time();
            if (timeout_expired(echo_rising_edge, max_time_taken))  // check for timeout
                portEXIT_CRITICAL(&mux);                            // exit back to task sceduling due to ping time out
        }
        portEXIT_CRITICAL(&mux);                                    // done, exiting back to task scheduling
        uint32_t time_taken = time - echo_rising_edge;              // calculate the time it took for the pulse to travel distance*2
        float distance = time_taken / ROUNDTRIP;                    // calculate distance (one way only)
        readings[i] = distance;
    }
    float totDist = 0;
    float avgDistance;
    for(int i = 0;i<5;i++){

        totDist = totDist + readings[i];
    }
    avgDistance = totDist/5;
    return avgDistance;
}