/* i2c - Simple example

   Simple I2C example that shows how to initialize I2C
   as well as reading and writing from and to registers for a sensor connected over I2C.

   The sensor used in this example is a MPU9250 inertial measurement unit.

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "i2c_simple/Library/APDS9960.h"
#include <portmacro.h>

static const char *TAG = "i2c-simple-example";

//#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
//#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_SCL_IO           21      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           20      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define MPU9250_SENSOR_ADDR                 0x68        /*!< Slave address of the MPU9250 sensor */
#define MPU9250_WHO_AM_I_REG_ADDR           0x75        /*!< Register addresses of the "who am I" register */

#define MPU9250_PWR_MGMT_1_REG_ADDR         0x6B        /*!< Register addresses of the power managment register */
#define MPU9250_RESET_BIT                   7


#define APDS9960_SENSOR_ADDR 0x39


struct SparkFun_APDS9960
{
    int gesture_ud_delta_;
    int gesture_lr_delta_;
    
    int gesture_ud_count_;
    int gesture_lr_count_;
    
    int gesture_near_count_;
    int gesture_far_count_;
    
    int gesture_state_;
    int gesture_motion_;
};




// ------------------------ Function Definitions -----------------------------


void proximitySensing(){

     // Adjust the Proximity sensor gain
    if ( !setProximityGain(PGAIN_2X) ) {
        ESP_LOGI(TAG, "Something went wrong trying to set PGAIN");
    }
  
    // Start running the APDS-9960 proximity sensor (no interrupts)
    if ( enableProximitySensor(false) ) {
        ESP_LOGI(TAG, "Proximity sensor is now running");
    } else {
        ESP_LOGI(TAG, "Something went wrong during sensor init!");
    }

    uint8_t proximity_data = 0;
    while(1) {
        if ( !readProximity(proximity_data) ) {
             ESP_LOGI(TAG, "Error reading proximity value");
        } else {
             ESP_LOGI(TAG, "Proximity: %d", proximity_data);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }


}

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void app_main(void)
{

    struct SparkFun_APDS9960 initObj;

    initObj.gesture_ud_delta_ = 0;
    initObj.gesture_lr_delta_ = 0;
    
    initObj.gesture_ud_count_ = 0;
    initObj.gesture_lr_count_ = 0;
    
    initObj.gesture_near_count_ = 0;
    initObj.gesture_far_count_ = 0;
    
    initObj.gesture_state_ = 0;
    initObj.gesture_motion_ = 0;

    init();

    proximitySensing();

}
