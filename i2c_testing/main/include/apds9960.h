#ifndef APDS9960_H
#define APDS9960_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "top_level_i2c.h"

void apds9960_init();
void apds9960_read_rgb(uint16_t *r, uint16_t *g, uint16_t *b);
bool colourReading_left();
bool colourReading_right();

#endif
