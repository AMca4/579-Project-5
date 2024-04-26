#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO    22    /*!< GPIO number for I2C master clock */
#define I2C_MASTER_SDA_IO    21    /*!< GPIO number for I2C master data  */
#define I2C_MASTER_NUM       I2C_NUM_0  /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ    400000   /*!< I2C master clock frequency (400 kHz) */

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

void i2c_scan() {
    printf("Scanning I2C bus...\n");
    esp_err_t ret;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (ret != ESP_OK) {
        printf("Failed to configure I2C parameters\n");
        return;
    }
    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        printf("Failed to install I2C driver\n");
        return;
    }

    for (int i = 0; i < 128; i++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 50 / portTICK_PERIOD_MS);
        if (ret == ESP_OK) {
            printf("Found device at address 0x%X\n", i);
        }
        i2c_cmd_link_delete(cmd);
        vTaskDelay(10 / portTICK_PERIOD_MS); // delay between attempts
    }
    printf("Scanning complete.\n");
}

esp_err_t i2c_master_init() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    return i2c_param_config(I2C_MASTER_NUM, &conf);
}

esp_err_t i2c_master_sensor_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_wr, size_t size) {
     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write(cmd, data_wr, size, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t i2c_master_sensor_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_rd, size_t size) {
      i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

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

void apds9960_task(void *pvParameters) {
    i2c_scan(); // Scan the I2C bus before initializing the APDS9960 sensor
    i2c_master_init();
    apds9960_init();
    while(1) {
        uint16_t r, g, b;
        apds9960_read_rgb(&r, &g, &b);
        printf("Red: %d, Green: %d, Blue: %d\n", r, g, b);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    xTaskCreate(&apds9960_task, "apds9960_task", 4096, NULL, 5, NULL);
}