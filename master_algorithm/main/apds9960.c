#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "include/apds9960.h"
#include "i2c_top_level.h"

#define I2C_MASTER_SCL_IO    22    /*!< GPIO number for I2C master clock */
#define I2C_MASTER_SDA_IO    21    /*!< GPIO number for I2C master data  */
#define APDS9960_I2C_ADDR     0x39   /*!< I2C address for APDS9960 sensor */

#define APDS9960_REG_ENABLE   0x80
#define APDS9960_REG_ATIME    0x82
#define APDS9960_REG_CONTROL  0x8F
#define APDS9960_REG_STATUS   0x93
#define APDS9960_REG_CDATAL   0x94
#define APDS9960_REG_CDATAH   0x95
#define APDS9960_REG_RDATAL   0x96
#define APDS9960_REG_RDATAH   0x97
#define APDS9960_REG_GDATAL   0x98
#define APDS9960_REG_GDATAH   0x99
#define APDS9960_REG_BDATAL   0x9A
#define APDS9960_REG_BDATAH   0x9B

struct colourData {

    uint16_t red, green, blue;

};

void apds9960_init() {
     // Enable the sensor
    //uint8_t enable = 0x01;
    uint8_t enable = 0x03;
    i2c_master_sensor_write(APDS9960_I2C_ADDR, APDS9960_REG_ENABLE, &enable, 1);
    // Set integration time (adjust as needed)
    uint8_t atime = 0xFF; // max value
    i2c_master_sensor_write(APDS9960_I2C_ADDR, APDS9960_REG_ATIME, &atime, 1);
    // Enable RGB, proximity, and gesture sensing
    uint8_t control = 0x0F;
    i2c_master_sensor_write(APDS9960_I2C_ADDR, APDS9960_REG_CONTROL, &control, 1);
}

void apds9960_read_rgb(uint16_t *r, uint16_t *g, uint16_t *b) {
     uint8_t data[6];
    i2c_master_sensor_read(APDS9960_I2C_ADDR, APDS9960_REG_RDATAL, data, 6);
    *r = (data[1] << 8) | data[0];
    *g = (data[3] << 8) | data[2];
    *b = (data[5] << 8) | data[4];
}

bool colourReading() {
    struct colourData reading;
    //i2c_master_initOLD();
    apds9960_init();
    uint16_t r, g, b;
    apds9960_read_rgb(&r, &g, &b);
    printf("LEFT: R: %d, G: %d, B: %d\n", r, g, b);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    reading.red = r;
    reading.green = g;
    reading.blue = b;
    
    if (reading.red + reading.blue + reading.green > 50){ // Target is not Black
        return false;
    } else{ // Target is assumed white
        return true;
    }

}   

