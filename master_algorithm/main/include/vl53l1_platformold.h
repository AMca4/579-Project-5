#ifndef VL53L1_PLATFORM_H
#define VL53L1_PLATFORM_H

#include "include/vl53l1_api.h"

VL53L1_Error VL53L1_WaitDeviceBooted(VL53L1_Dev_t *pDevice);
VL53L1_Error VL53L1_DataInit(VL53L1_Dev_t *pDevice);
VL53L1_Error VL53L1_StaticInit(VL53L1_Dev_t *pDevice);
VL53L1_Error VL53L1_PerformRefCalibration(VL53L1_Dev_t *pDevice);
VL53L1_Error VL53L1_GetDistance(VL53L1_Dev_t *pDevice, uint16_t *pDistance);
VL53L1_Error VL53L1_PerformSingleRangingMeasurement(pDevice, NULL);

#endif /* VL53L1_PLATFORM_H */