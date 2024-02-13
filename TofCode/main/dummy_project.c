#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

#define I2C_MASTER_SCL_IO           22        // gpio number for I2C master clock
#define I2C_MASTER_SDA_IO           21        // gpio number for I2C master data
#define I2C_MASTER_NUM              I2C_NUM_0 // I2C port number for master dev
#define I2C_MASTER_FREQ_HZ          100000    // I2C master clock frequency
#define VL53L0X_DEFAULT_ADDRESS     0x29      // VL53L0X default I2C address

void i2c_master_init() {
    i2c_config_t conf;
   
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void app_main() {
    i2c_master_init();
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    VL53L0X_Dev_t dev;
    VL53L0X_RangingMeasurementData_t RangingMeasurementData;

    dev.i2c_address = VL53L0X_DEFAULT_ADDRESS;

    Status = VL53L0X_DataInit(&dev);
    if (Status != VL53L0X_ERROR_NONE) {
        printf("VL53L0X_DataInit failed\n");
        return;
    }

    Status = VL53L0X_StaticInit(&dev);
    if (Status != VL53L0X_ERROR_NONE) {
        printf("VL53L0X_StaticInit failed\n");
        return;
    }

    while (1) {
        Status = VL53L0X_PerformSingleRangingMeasurement(&dev, &RangingMeasurementData);
        if (Status != VL53L0X_ERROR_NONE) {
            printf("VL53L0X_PerformSingleRangingMeasurement failed\n");
            continue;
        }

        printf("Distance: %d mm\n", RangingMeasurementData.RangeMilliMeter);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 second
    }
}