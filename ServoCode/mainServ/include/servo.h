#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (5)
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT
#define LEDC_DUTY               (4096)
#define LEDC_FREQUENCY          (50)


void example_ledc_init(void);

#endif /* SERVO_H_ */