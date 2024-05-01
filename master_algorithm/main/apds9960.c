#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "apds9960.h"
#include "i2c_master.h"

#define I2C_MASTER_SCL_IO    22    /*!< GPIO number for I2C master clock */
#define I2C_MASTER_SDA_IO    21    /*!< GPIO number for I2C master data  */
#define APDS9960_I2C_ADDR     0x39   /*!< I2C address for APDS9960 sensor */

#define APDS9960_REG_ENABLE   128
#define APDS9960_REG_ATIME    200
#define APDS9960_REG_CONTROL  143
#define APDS9960_REG_STATUS   147
#define APDS9960_REG_CDATAL   148
#define APDS9960_REG_CDATAH   149
#define APDS9960_REG_RDATAL   150
#define APDS9960_REG_RDATAH   151
#define APDS9960_REG_GDATAL   152
#define APDS9960_REG_GDATAH   153
#define APDS9960_REG_BDATAL   154
#define APDS9960_REG_BDATAH   155

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
    apds9960_init();
    uint16_t r, g, b;
    apds9960_read_rgb(&r, &g, &b);
    printf("Red: %d, Green: %d, Blue: %d\n", r, g, b);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    reading.red = r;
    reading.green = g;
    reading.blue = b;

    if ((reading.red + reading.blue + reading.green) > 50){ // Target is not Black
        return false;
    } else{ // Target is assumed white
        return true;
    }

}   


