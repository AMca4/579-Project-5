#include "include/kinematics.h"

#define forward_pin GPIO_NUM_32
#define reverse_pin GPIO_NUM_33;
#define fast_freq 10000;
#define normal_freq 7500;
#define slow_freq 5000;
#define tyre_circumference 89.5354;

struct pwm_details{
    int32_t frequency;
    float duty_cycle;
    gpio_num_t pin;
    float duration;
};

void forward(float meters) {
    struct pwm_details pwm_slow_forward;
    pwm_slow_forward.duty_cycle = 0.5;         // gives an output voltage of 2.5 V
    pwm_slow_forward.frequency = slow_freq;
    pwm_slow_forward.pin = forward_pin;
    pwm_slow_forward.duration = meters / tyre_circumference;
    // pass this struct to a pwm function ig
}

void fast_forward(float meters){
    struct pwm_details pwm_fast_forward;
    pwm_fast_forward.duty_cycle = 0.8;
    pwm_fast_forward.frequency = fast_freq;
    pwm_fast_forward.pin = forward_pin;
    pwm_fast_forward.duration = meters / tyre_circumference;
    // pass this struct to a pwm function ig
}

void reverse(float meters){
    struct pwm_details pwm_reverse;
    pwm_reverse.duty_cycle = 0.64;         // gives an output voltage of 3.2 V
    pwm_reverse.frequency = normal_freq;
    pwm_reverse.pin = reverse_pin;
    pwm_reverse.duration = meters / tyre_circumference;
    // pass this struct to a pwm function ig
}

void brake(){
    // don't know how to code that bruva 
}
