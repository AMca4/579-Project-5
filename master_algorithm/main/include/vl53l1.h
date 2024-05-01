#ifndef TOF_SENSOR_H
#define TOF_SENSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
//#include "esp_event_loop.h"
#include "esp_partition.h"

#include "nvs_flash.h"
#include "nvs.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"

#include "soc/gpio_struct.h"

#include "vl53l1_platform.h"
#include "vl53l1_platform_user_data.h"
#include "vl53l1_api.h"

#define I2C_MASTER_SCL_IO               22
#define I2C_MASTER_SDA_IO               21
#define I2C_MASTER_NUM                  I2C_NUM_0
#define I2C_MASTER_TX_BUF_DISABLE       0
#define I2C_MASTER_RX_BUF_DISABLE       0
#define I2C_MASTER_FREQ_HZ              400000

extern VL53L1_Dev_t dev;
extern VL53L1_DEV Dev;


void initialiseTOF();
void tofOperationError(VL53L1_Error Status, char *Msg);
void tofReadingError(VL53L1_Error Status, char *Msg);
void f_i2c_init(void);
VL53L1_Error tofInit(VL53L1_DEV Dev);
int tofReadingfunc();
int tofReading();

#endif /* TOF_SENSOR_H */