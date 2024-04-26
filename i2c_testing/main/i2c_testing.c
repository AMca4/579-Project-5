#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000


#define ADXL345_ADDR 0x53
#define APDS9960_ADDR 0x39

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

#define PCA9546A_ADDR 0x70

#define LED GPIO_NUM_2
#define CHANNEL_FOR_ACC 0
#define CHANNEL_FOR_COLOR_LEFT 1
#define CHANNEL_FOR_TOF 2
#define CHANNEL_FOR_COLOR RIGHT 3

esp_err_t i2c_master_init() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = (0);
    i2c_param_config(I2C_MASTER_NUM, &conf);
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

esp_err_t select_channel(uint8_t channel) {
    uint8_t data = 1 << channel;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, PCA9546A_ADDR << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, &data, 1, true); // Write data byte
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}

void apds9960_init() {
    uint8_t enable = 0x03;
    i2c_master_write(APDS9960_I2C_ADDR, APDS9960_REG_ENABLE, &enable, 1);
    // Set integration time (adjust as needed)
    uint8_t atime = 0xFF; // max value
    i2c_master_write(APDS9960_I2C_ADDR, APDS9960_REG_ATIME, &atime, 1);
    // Enable RGB, proximity, and gesture sensing
    uint8_t control = 0x0F;
    i2c_master_write(APDS9960_I2C_ADDR, APDS9960_REG_CONTROL, &control, 1);
}

void apds9960_read_color(uint16_t *r, uint16_t *g, uint16_t *b) {
    uint8_t data[6];
    i2c_master_read(APDS9960_I2C_ADDR, APDS9960_REG_RDATAL, data, 6);
    *r = (data[1] << 8) | data[0];
    *g = (data[3] << 8) | data[2];
    *b = (data[5] << 8) | data[4];
}


void app_main() {
    i2c_master_init();
    // gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    
    while(1) {
        // Select channel for APDS9960
        select_channel(CHANNEL_FOR_COLOR_LEFT);
        // Initialize APDS9960
        apds9960_init();
        // Read color data
        uint16_t red, green, blue;
        apds9960_read_color(&red, &green, &blue);

        printf("Red: %d, Green: %d, Blue: %d\n", red, green, blue);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Adjust delay as needed
    }
}
