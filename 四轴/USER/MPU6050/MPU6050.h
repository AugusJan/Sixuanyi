#ifndef _IIC_MPU6050_H_   
#define _IIC_MPU6050_H_  

#include "stm32f10x.h"  

extern int16_t X_A;
extern int16_t Y_A;
extern int16_t Z_A;


extern float  X_A_av;
extern float  Y_A_av;
extern float  Z_A_av;

extern int16_t X_G;
extern int16_t Y_G;
extern int16_t Z_G;

#define MPU6050_SLAVE_ADDRESS 0xD0

//****************************************
// MPU6050
//****************************************
#define	SMPLRT_DIV		0x19	
#define	MPU_CONFIG		0x1A	
#define	GYRO_CONFIG		0x1B	
#define	ACCEL_CONFIG	0x1C	
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	
#define	WHO_AM_I		  0x75	



void  InitMPU6050(void);						
void GET_MPU_DATA(void);

#endif 
