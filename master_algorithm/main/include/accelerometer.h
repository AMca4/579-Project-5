#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#define I2C_MASTER_NUM I2C_NUM_0
#define ADXL345_ADDR 0x53
#define LED GPIO_NUM_2


void adxl345_read_acceleration(int16_t *z, int16_t *y) {
    uint8_t data[6]; // Read 6 bytes for x, y, and z axes
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ADXL345_ADDR << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x32, true); // Start reading from register 0x32
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ADXL345_ADDR << 1 | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 6, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    // Combine the high and low bytes for z and y axes
    *z = (data[5] << 8) | data[4];
    *y = (data[3] << 8) | data[2];
}

void calibrate_accelerometer(int16_t *z_offset, int16_t *y_offset) {
    // Perform calibration by averaging multiple readings with the sensor at rest
    int16_t z_sum = 0;
    int16_t y_sum = 0;
    const int num_samples = 10;
    for (int i = 0; i < num_samples; i++) {
        int16_t z, y;
        adxl345_read_acceleration(&z, &y);
        z_sum += z;
        y_sum += y;
        vTaskDelay(pdMS_TO_TICKS(10)); // Delay between readings
    }
    *z_offset = z_sum / num_samples;
    *y_offset = y_sum / num_samples;
}

/*
void app_main() {
    i2c_master_init();
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    
    int16_t z_offset, y_offset;
    calibrate_accelerometer(&z_offset, &y_offset);

    while(1) {
        // Read accelerometer data
        int16_t z_raw, y_raw;
        adxl345_read_acceleration(&z_raw, &y_raw);

        // Apply calibration offsets
        int16_t z = z_raw - z_offset;
        int16_t y = y_raw - y_offset;

        // Check for motion
        if (abs(z) > 30 || abs(y) > 30) {
            gpio_set_level(LED, 1);
            vTaskDelay(pdMS_TO_TICKS(200));
            gpio_set_level(LED, 0);
        }
        else {
            gpio_set_level(LED, 0);
        }

        printf(" Z-axis: %d, Y-axis: %d\n", z, y);
        vTaskDelay(pdMS_TO_TICKS(400)); 
    }
}
*/