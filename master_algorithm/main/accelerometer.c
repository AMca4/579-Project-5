#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "accelerometer.h"

#define I2C_MASTER_SCL_IO 22       
#define I2C_MASTER_SDA_IO 21       
#define I2C_MASTER_NUM I2C_NUM_0   
#define I2C_MASTER_FREQ_HZ 100000  
#define ADXL345_ADDR 0x53          

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

void adxl345_init() {
    // Enable measurement mode
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ADXL345_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x2D, true); // Power control register
    i2c_master_write_byte(cmd, 0x08, true); // Measure enable
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
}

void adxl345_read_acceleration(int16_t *x, int16_t *y) {
    uint8_t data[4];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ADXL345_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x32, true); // Start reading from register 0x32
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ADXL345_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 4, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    // Convert data to 16-bit integers
    *x = (data[1] << 8) | data[0];
    *y = (data[3] << 8) | data[2];
}

// void app_main() {

    
//     int16_t x1, y1, x2, y2;

//     while(1) {
//         adxl345_read_acceleration(&x1, &y1);
//         vTaskDelay(pdMS_TO_TICKS(150)); 
//         adxl345_read_acceleration(&x2, &y2);
//         if (((x2-x1) > 30) | ((x1-x2) > 30 )) {
//             gpio_set_level(LED, 1);
//             vTaskDelay(pdMS_TO_TICKS(200));
//             gpio_set_level(LED, 0);
//         }
//         else if(((y2-y1) > 30) | ((y1-y2) > 30)){
//             gpio_set_level(LED, 1);
//             vTaskDelay(pdMS_TO_TICKS(50));
//             gpio_set_level(LED, 0);
//             vTaskDelay(pdMS_TO_TICKS(50));
//             gpio_set_level(LED, 1);
//             vTaskDelay(pdMS_TO_TICKS(50));
//             gpio_set_level(LED, 0);
//         }
//         printf("X-axis: %d, Y-axis: %d\n", x2, y2);
//         vTaskDelay(pdMS_TO_TICKS(100)); 
//     }
// }