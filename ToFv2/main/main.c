#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_partition.h"

#include "nvs_flash.h"
#include "nvs.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"

#include "soc/gpio_struct.h"

#include "include/vl53l1_platform.h"
#include "include/vl53l1_platform_user_data.h"
#include "include/vl53l1_api.h"


#define I2C_MASTER_SCL_IO               22
#define I2C_MASTER_SDA_IO               21
#define I2C_MASTER_NUM                  I2C_NUM_0
#define I2C_MASTER_TX_BUF_DISABLE       0
#define I2C_MASTER_RX_BUF_DISABLE       0
#define I2C_MASTER_FREQ_HZ              400000

VL53L1_Dev_t 	dev;
VL53L1_DEV 		Dev 		= &dev;



void app_main(void)
{
    int 					Blink = 0, Stat;
 	VL53L1_Error 			Status;
    nvs_flash_init();
    i2cInit();

    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    Status = tofInit(Dev); // 
    if (Status != VL53L1_ERROR_NONE)	tofOperationError(Status, "vl53l1_init");

    printf("Init sensor Ok. \n");
    while (	1 )
		{
		vTaskDelay(pdMS_TO_TICKS(100));
		Blink = !Blink;
		gpio_set_level(GPIO_NUM_2, Blink);
		Stat = tofReading();
		if (Stat == -1)
			{
		    Status = tofInit(Dev); // Attempt to reinitialise sensor
		    if (Status != VL53L1_ERROR_NONE)	printf("Reinitialisation of ToF Sensor Failes"); // If fails flag error
			}
		else
			{
			printf("Distance =%d mm\n", Stat);
			}
		}
}