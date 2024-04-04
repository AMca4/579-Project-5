#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

esp_err_t i2c_master_init();

void adxl345_init();

void adxl345_read_acceleration(int16_t *x, int16_t *y);

#endif