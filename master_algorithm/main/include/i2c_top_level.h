#ifndef I2C_TOP_LEVEL_H
#define I2C_TOP_LEVEL_H

#include <stdio.h>
#include "esp_err.h"

#define I2C_MASTER_FREQ_HZ          400000 
#define I2C_MASTER_TX_BUF_DISABLE   0  
#define I2C_MASTER_RX_BUF_DISABLE   0  
#define I2C_MASTER_TIMEOUT_MS       1000




void i2c_master_initNew();
void i2c_scan();
esp_err_t i2c_master_sensor_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_wr, size_t size);
esp_err_t i2c_master_sensor_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_rd, size_t size);



#endif