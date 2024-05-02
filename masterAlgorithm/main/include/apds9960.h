#ifndef APDS9960_H
#define APDS9960_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

void i2c_scan();
esp_err_t i2c_master_init();
esp_err_t i2c_master_sensor_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_wr, size_t size);
esp_err_t i2c_master_sensor_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_rd, size_t size);
void apds9960_init();
void apds9960_read_rgb(uint16_t *r, uint16_t *g, uint16_t *b);
bool colourReading();

#endif
#ifndef APDS9960_H
#define APDS9960_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

void i2c_scan();
esp_err_t i2c_master_init();
esp_err_t i2c_master_sensor_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_wr, size_t size);
esp_err_t i2c_master_sensor_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_rd, size_t size);
void apds9960_init();
void apds9960_read_rgb(uint16_t *r, uint16_t *g, uint16_t *b);
bool colourReading();

#endif