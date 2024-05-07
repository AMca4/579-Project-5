// i2c_top_level.h

#include <stdio.h>
#include "esp_err.h"

#ifndef I2C_MASTER_H
#define I2C_MASTER_H


#define I2C_MASTER_FREQ_HZ          400000 
#define I2C_MASTER_TX_BUF_DISABLE   0  
#define I2C_MASTER_RX_BUF_DISABLE   0  
#define I2C_MASTER_TIMEOUT_MS       1000

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0

#define ADXL345_ADDR 0x53
#define APDS9960_ADDR 0x39
#define PCA9546A_ADDR 0x70

#define LED GPIO_NUM_2
#define CHANNEL_FOR_ACC 0
#define CHANNEL_FOR_COLOR_LEFT 1
#define CHANNEL_FOR_TOF 2
#define CHANNEL_FOR_COLOR_RIGHT 3



void i2c_master_initNew();
esp_err_t i2c_master_initOLD(); // Mikes not included this is it used??
void i2c_scan();
esp_err_t i2c_master_sensor_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_wr, size_t size);
esp_err_t i2c_master_sensor_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_rd, size_t size);
esp_err_t select_channel(uint8_t channel);


#endif