#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#define I2C_MASTER_NUM I2C_NUM_0
#define ADXL345_ADDR 0x53
#define LED GPIO_NUM_2

void adxl345_read_acceleration(int16_t *z, int16_t *y);
void calibrate_accelerometer(int16_t *z_offset, int16_t *y_offset);

