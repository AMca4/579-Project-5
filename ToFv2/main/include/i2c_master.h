// i2c_master.h

#include <stdio.h>
#include "esp_err.h"

#ifndef I2C_MASTER_H
#define I2C_MASTER_H


#define I2C_MASTER_FREQ_HZ          400000 
#define I2C_MASTER_TX_BUF_DISABLE   0  
#define I2C_MASTER_RX_BUF_DISABLE   0  
#define I2C_MASTER_TIMEOUT_MS       1000



esp_err_t i2c_master_init(int i2c_master_port, int sda_io, int scl_io);

#endif