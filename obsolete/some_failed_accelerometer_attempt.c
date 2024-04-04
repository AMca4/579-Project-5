
// // #include <stdio.h>
// // #include <stdbool.h>
// // #include <freertos/FreeRTOS.h>
// // #include <freertos/task.h>
// // // #include <esp_err.h>
// // #include "driver/gpio.h"
// // #include <esp_timer.h>
// // #include "esp32/rom/ets_sys.h"
// // #include "driver/i2c_slave.h"

// // //--------adxl345 address locations--------//
// // #define POWER_CTL 0x2D
// // #define DATA_X0 0x32
// // #define DATA_X1 0x33
// // #define DATA_Y0 0x34
// // #define DATA_Y1 0x35
// // #define DATA_Z0 0x36
// // #define DATA_Z1 0x37

// // //--------sda and scl locations--------//
// // #define sda_pin GPIO_NUM_21
// // #define scl_pin GPIO_NUM_22

// // int ADXL_address = 0x53; // default adxl 345 location
// // float X0, X1, X_out;
// // float Y0, Y1, Y_out;
// // float Z0, Z1, Z_out;

// // void init_GPIO(){
// //     gpio_set_direction(sda_pin,GPIO_MODE_INPUT_OUTPUT);
// //     gpio_set_direction(scl_pin,GPIO_MODE_INPUT_OUTPUT);
// //     gpio_set_pull_mode(sda_pin,GPIO_PULLUP_ONLY);
// //     gpio_set_pull_mode(scl_pin,GPIO_PULLUP_ONLY);
// // }


// // void init_Accelerometer(uint8_t sda_pin, uint8_t scl_pin){
// //     i2c_slave_config_t adxl_config = {
// //         .addr_bit_len = I2C_ADDR_BIT_10,   // 10-bit addresses
// //         .clk_source = I2C_CLK_SRC_DEFAULT, // default clock source
// //         .i2c_port = -1,                    //-1 for auto-select
// //         .send_buf_depth = 256,           // need to look this up
// //         .scl_io_num = scl_pin,
// //         .sda_io_num = sda_pin,
// //         .slave_addr = ADXL_address,
// //     };

// //     i2c_slave_dev_handle_t adxl_bus_handle;

// //     ESP_ERROR_CHECK(i2c_new_slave_device(&adxl_config, &adxl_bus_handle));


// //     //------ from esp32 documentation-------------

// //     // static IRAM_ATTR bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *user_data){
// //     //     BaseType_t high_task_wakeup = pdFALSE;
// //     //     QueueHandle_t receive_queue = (QueueHandle_t)user_data;
// //     //     xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
// //     //     return high_task_wakeup == pdTRUE;
// //     // }

// //     // uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH);
// //     // uint32_t size_rd = 0;

// //     // i2c_slave_config_t i2c_slv_config = {
// //     //     .addr_bit_len = I2C_ADDR_BIT_LEN_7,
// //     //     .clk_source = I2C_CLK_SRC_DEFAULT,
// //     //     .i2c_port = TEST_I2C_PORT,
// //     //     .send_buf_depth = 256,
// //     //     .scl_io_num = I2C_SLAVE_SCL_IO,
// //     //     .sda_io_num = I2C_SLAVE_SDA_IO,
// //     //     .slave_addr = 0x58,
// //     // };

// //     // i2c_slave_dev_handle_t slave_handle;
// //     // ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

// //     // s_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
// //     // i2c_slave_event_callbacks_t cbs = {
// //     //     .on_recv_done = i2c_slave_rx_done_callback,
// //     // };
// //     // ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, s_receive_queue));

// //     // i2c_slave_rx_done_event_data_t rx_data;
// //     // ESP_ERROR_CHECK(i2c_slave_receive(slave_handle, data_rd, DATA_LENGTH));
// //     // xQueueReceive(s_receive_queue, &rx_data, pdMS_TO_TICKS(10000));
// //     // // Receive done.
// // }


// // void app_main(void){
// //     //-----initialisations------//
// //     vns_flash_init();
// //     init_GPIO();
// //     init_Accelerometer(sda_pin, scl_pin);
// //     while (1){
// //         // esp_err_t i2c_slave_receive(i2c_slave_dev_handle_t i2c_slave, uint8_t *data, size_t receive_size);
// //         printf("X: %f , Y: %f, Z: %f\n", X_out, Y_out, Z_out);
// //         delay(pdMS_TO_TICKS(500));
// //     }
// // }
