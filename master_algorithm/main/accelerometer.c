#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define PCA9546A_ADDR 0x70

esp_err_t i2c_master_init() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
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

void app_main() {
    i2c_master_init();
    
    for (int channel = 0; channel < 4; channel++) {
        printf("Selecting channel %d\n", channel);
        esp_err_t ret = select_channel(channel);
        if (ret == ESP_OK) {
            printf("Channel selected successfully.\n");
        } else {
            printf("Failed to select channel. Error code: %d\n", ret);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
