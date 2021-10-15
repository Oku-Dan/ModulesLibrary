#ifndef INC_MPU9250_H_
#define INC_MPU9250_H_

#include "stdint.h"
#include "stdbool.h"

#define MPU9250_ADDRESS_AD0_L 0x68
#define MPU9250_ADDRESS_AD0_H 0x69

#define MPU9250_WHO_AM_I 0x75
#define MPU9250_WHO_AM_I_DEFAULT 0x71
#define MPU9255_WHO_AM_I_DEFAULT 0x73

#define MPU9250_PWR_MGMT_1 0x6B
#define MPU9250_INT_PIN_CFG 0x37

#define MPU9250_ACCEL_CONFIG 0x1C
#define MPU9250_ACCEL_FS_SEL_2G 0x00
#define MPU9250_ACCEL_FS_SEL_4G 0x08
#define MPU9250_ACCEL_FS_SEL_8G 0x10
#define MPU9250_ACCEL_FS_SEL_16G 0x18

#define MPU9250_GYRO_CONFIG 0x1B
#define MPU9250_GYRO_FS_SEL_250DPS 0x00
#define MPU9250_GYRO_FS_SEL_500DPS 0x08
#define MPU9250_GYRO_FS_SEL_1000DPS 0x10
#define MPU9250_GYRO_FS_SEL_2000DPS 0x18

#define MPU9250_ACCEL_XOUT_H 0x3B
#define MPU9250_ACCEL_XOUT_L 0x3C
#define MPU9250_ACCEL_YOUT_H 0x3D
#define MPU9250_ACCEL_YOUT_L 0x3E
#define MPU9250_ACCEL_ZOUT_H 0x3F
#define MPU9250_ACCEL_ZOUT_L 0x40
#define MPU9250_TEMP_OUT_H 0x41
#define MPU9250_TEMP_OUT_L 0x42
#define MPU9250_GYRO_XOUT_H 0x43
#define MPU9250_GYRO_XOUT_L 0x44
#define MPU9250_GYRO_YOUT_H 0x45
#define MPU9250_GYRO_YOUT_L 0x46
#define MPU9250_GYRO_ZOUT_H 0x47
#define MPU9250_GYRO_ZOUT_L 0x48

#define AK8963_ADDRESS 0x0C
#define AK8963_WIM 0x00
#define AK8963_WHI_DEFAULT 0x48
#define AK8963_CNTL1 0x0A
#define AK8963_CNTL1_MODE_SEL_8HZ 0x12
#define AK8963_CNTL1_MODE_SEL_100HZ 0x16
#define AK8963_ST1 0x02
#define AK8963_ASAX 0x10
#define AK8963_ASAY 0x11
#define AK8963_ASAZ 0x12

#define AK8963_HXL 0x03
#define AK8963_HXH 0x04
#define AK8963_HYL 0x05
#define AK8963_HYH 0x06
#define AK8963_HZL 0x07
#define AK8963_HZH 0x08

//---------------------------------------------
#define MPU9250_ACCEL_SCALE_2G 2
#define MPU9250_ACCEL_SCALE_4G 4
#define MPU9250_ACCEL_SCALE_8G 8
#define MPU9250_ACCEL_SCALE_16G 16

#define MPU9250_GYRO_SCALE_250DPS 250
#define MPU9250_GYRO_SCALE_500DPS 500
#define MPU9250_GYRO_SCALE_1000DPS 1000
#define MPU9250_GYRO_SCALE_2000DPS 2000

#define MPU9250_MAG_RATE_8Hz 8
#define MPU9250_MAG_RATE_100Hz 100
//---------------------------------------------

bool MPU9250_Initialize(int AccelScale, int GyroScale, int MagnetRate);
void MPU9250_ReadAccGyr(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);
void MPU9250_ReadAcc(float *ax, float *ay, float *az);
void MPU9250_ReadGyr(float *gx, float *gy, float *gz);
bool MPU9250_ReadMag(float *mx, float *my, float *mz);

#endif