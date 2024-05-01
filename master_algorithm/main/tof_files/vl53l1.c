#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VL53L1.h"

#define I2C_MASTER_SCL_IO               22
#define I2C_MASTER_SDA_IO               21
#define I2C_MASTER_NUM                  I2C_NUM_0
#define I2C_MASTER_TX_BUF_DISABLE       0
#define I2C_MASTER_RX_BUF_DISABLE       0
#define I2C_MASTER_FREQ_HZ              400000


VL53L1_Dev_t 	dev;
VL53L1_DEV 		Dev 		= &dev;


void initialiseTOF(){

	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	VL53L1_Error 			Status;
	Status = tofInit(Dev);
	if (Status != VL53L1_ERROR_NONE)	tofOperationError(Status, "vl53l1_init");

    printf("TOF Init sensor Ok. \n");

}

void	tofOperationError(VL53L1_Error 	Status, char *Msg)
	{
	printf("Error %s, status=%d \n", Msg, Status);
	printf("Error Initialising ToF VL53L1X Sensor.\n Attempting Reboot after 10 seconds");
	vTaskDelay(pdMS_TO_TICKS(10000));
	esp_restart();
	}

char Lastmsg[100] = "";
void	tofReadingError(VL53L1_Error 	Status, char *Msg)
	{
	if (! strcmp (Lastmsg, Msg) ) return;
	strcpy(Lastmsg, Msg);
	printf("Error %s, status=%d \n", Msg, Status);
	}

void 	f_i2c_init(void)
{
    i2c_config_t 	conf;
	int				Status;
    int i2c_master_port 	= I2C_MASTER_NUM;
    conf.mode 				= I2C_MODE_MASTER;
    conf.sda_io_num 		= I2C_MASTER_SDA_IO;
    conf.sda_pullup_en 		= GPIO_PULLUP_ENABLE;
    conf.scl_io_num 		= I2C_MASTER_SCL_IO;
    conf.scl_pullup_en 		= GPIO_PULLUP_ENABLE;
    conf.master.clk_speed 	= I2C_MASTER_FREQ_HZ;

    Status = i2c_param_config(i2c_master_port, &conf);
    if (Status != ESP_OK)	tofOperationError(Status, "i2c_param_config");

    printf("i2c_param_config - Ok. \n");


    Status = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (Status != ESP_OK)	tofOperationError(Status, "i2c_driver_install");

	printf("i2c_driver_install - Ok. \n");

}


VL53L1_Error 	tofInit(VL53L1_DEV 	Dev)
{
 	VL53L1_Error 	Status 	 		= VL53L1_ERROR_NONE;
    				Dev->I2cHandle 	= I2C_MASTER_NUM;
    				Dev->I2cDevAddr = 0x52;

	Status = VL53L1_WaitDeviceBooted(Dev);
	if (Status != VL53L1_ERROR_NONE)	tofOperationError(Status, "VL53L1_WaitDeviceBooted");

 	Status = VL53L1_DataInit(Dev);
 	if (Status != VL53L1_ERROR_NONE)	tofOperationError(Status, "VL53L1_DataInit");

 	Status = VL53L1_StaticInit(Dev);
 	if (Status != VL53L1_ERROR_NONE)	tofOperationError(Status, "VL53L1_StaticInit");

// 	printf("VL53L1_SetDistanceMode Inicio\n");
// 	VL53L1_SetDistancexMode(Dev, VL53L1_DISTANCEMODE_LONG);

 	Status = VL53L1_SetMeasurementTimingBudgetMicroSeconds(Dev, 50000);
 	if (Status != VL53L1_ERROR_NONE)	tofOperationError(Status, "VL53L1_SetMeasurementTimingBudgetMicroSeconds");

 	Status = VL53L1_SetInterMeasurementPeriodMilliSeconds(Dev, 500);
 	if (Status != VL53L1_ERROR_NONE)	tofOperationError(Status, "VL53L1_SetInterMeasurementPeriodMilliSeconds");

 	Status = VL53L1_StartMeasurement(Dev);
 	if (Status != VL53L1_ERROR_NONE)	tofOperationError(Status, "VL53L1_StartMeasurement");

 	return(Status);
}
int tofReadingfunc()
{
 	VL53L1_Error 						Status	= 999;
	//VL53L1_Error 						Status	= 1;
 	VL53L1_RangingMeasurementData_t		RangingData;
 	uint8_t 							pDataReady;
 	int									retry = 0;


 	while (Status != VL53L1_ERROR_NONE)
 		{
 	 	Status = VL53L1_GetMeasurementDataReady( Dev , &pDataReady);
 		if (Status != VL53L1_ERROR_NONE)
 			{
 			tofReadingError(Status, "VL53L1_GetMeasurementDataReady");
 			continue;
 			}

 	 	Status = VL53L1_GetRangingMeasurementData(Dev, &RangingData);
 		if (Status != VL53L1_ERROR_NONE)
 			{
 			tofReadingError(Status, "VL53L1_GetRangingMeasurementData");
 			continue;
 			}

 		Status = RangingData.RangeStatus;
 		if (Status != VL53L1_ERROR_NONE)
 			{
 			tofReadingError(Status, "VL53L1_GetRangingMeasurementData - RangeStatus");
 			}

 		retry++;
 		if (retry > 100)	return(-1);
 		}

	Status = VL53L1_ClearInterruptAndStartMeasurement( Dev );
	if (Status != VL53L1_ERROR_NONE)	tofReadingError(Status, "VL53L1_ClearInterruptAndStartMeasurement");

 	memset(Lastmsg,0,sizeof(Lastmsg));

	return (RangingData.RangeMilliMeter);
}

int tofReading()
{
    int 					Blink = 0, Stat;
 	VL53L1_Error 			Status;


    nvs_flash_init();
    //f_i2c_init();

    //gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    //Status = tofInit(Dev);
    //if (Status != VL53L1_ERROR_NONE)	tofOperationError(Status, "vl53l1_init");

    //printf("TOF Init sensor Ok. \n");
		
	vTaskDelay(pdMS_TO_TICKS(200));				// Polling Time 

	Blink = !Blink;
	gpio_set_level(GPIO_NUM_2, Blink);

	Stat = tofReadingfunc();
	if (Stat == -1)
		{
		Status = tofInit(Dev);
		if (Status != VL53L1_ERROR_NONE)	tofOperationError(Status, "vl53l1_init");
			printf("Error Obtaining TOF READING");
		}
	else
		{
		//printf("Distance =%d mm\n", Stat);
		printf("TOF Reading obtained \n");
		}

		return Stat;
		
}