#include "MPU6050.h"
#include "IIC.h"
#include "imu.h"

//**************************************
//MPU6050
//**************************************
void InitMPU6050(void)
{
	IIC_GPIO_Config(); 
	Delay_us(5000);
	Single_WriteI2C(MPU6050_SLAVE_ADDRESS,PWR_MGMT_1  , 0x08); //失能温度
	Delay_us(5000);
	Single_WriteI2C(MPU6050_SLAVE_ADDRESS,MPU_CONFIG  , 0x04);	//21hz滤波
	Delay_us(5000);
	Single_WriteI2C(MPU6050_SLAVE_ADDRESS,SMPLRT_DIV  , 0x04); //1khz  200hz
	Delay_us(5000);
	Single_WriteI2C(MPU6050_SLAVE_ADDRESS,GYRO_CONFIG , 0x18); //+-2000度/s
	Delay_us(5000);
	Single_WriteI2C(MPU6050_SLAVE_ADDRESS,ACCEL_CONFIG, 0x08); //+-4g/s
	Delay_us(5000);
}
//**************************************
int16_t GetData(uint8_t REG_Address)
{
	uint8_t Hd,Ld;
	Hd=Single_ReadI2C(MPU6050_SLAVE_ADDRESS,REG_Address);
	Ld=Single_ReadI2C(MPU6050_SLAVE_ADDRESS,REG_Address+1);
	return (Hd<<8)+Ld; 
}
//**************************************
//地推均值滤波
//**************************************
#define N 10
float Data_X_g[N];
float Data_Y_g[N];
float Data_Z_g[N];
float GildeAverageValueFilter(float NewValue,float *Data)
{
	float max,min;
	float sum;
	unsigned char i;
	Data[0]=NewValue;
	max=Data[0];
	min=Data[0];
	sum=Data[0];
	for(i=N-1;i!=0;i--)
	{
	  if(Data[i]>max) max=Data[i];
	  else if(Data[i]<min) min=Data[i];
	  sum+=Data[i];
	  Data[i]=Data[i-1]; 
	}       
	  i=N-2;
	 sum=sum-max-min+i/2;
	 sum=sum/i;
	 return(sum);
}
//********************************************************
/*
pitch:z轴和y轴
roll:z轴和x轴

*/

int16_t X_A;
int16_t Y_A;
int16_t Z_A;

float  X_A_av;
float  Y_A_av;
float  Z_A_av;

int16_t X_G;
int16_t Y_G;
int16_t Z_G;

void GET_ACC_DATA(void)
{

	X_A = GetData(ACCEL_XOUT_H);
	X_A_av = GildeAverageValueFilter(X_A,Data_X_g)-123;
	Y_A = GetData(ACCEL_YOUT_H);
	Y_A_av = GildeAverageValueFilter(Y_A,Data_Y_g)-30;
	Z_A = GetData(ACCEL_ZOUT_H);
	Z_A_av = GildeAverageValueFilter(Z_A,Data_Z_g);
	
}
void GET_GYRO_DATA(void)
{

	X_G = GetData(GYRO_XOUT_H)+31;
	Y_G = GetData(GYRO_YOUT_H)+6;
	Z_G = GetData(GYRO_ZOUT_H);
	
}
/*
float  X_g;
float  Y_g;
float  Z_g;

float  X_g_av;
float  Y_g_av;
float  Z_g_av;

float  X_w;
float  Y_w;
float  Z_w;

float  X_g_off = 0;
float  Y_g_off = 0;
float  Z_g_off = 0;

float  X_w_off = -13;
float  Y_w_off = -25;
float  Z_w_off = 41;

void GET_ACC_DATA(void)
{
	X_g    = GetData(ACCEL_XOUT_H) + X_g_off;
	X_g_av = GildeAverageValueFilter(X_g,Data_X_g);
	Y_g    = GetData(ACCEL_YOUT_H) + Y_g_off;
	Y_g_av = GildeAverageValueFilter(Y_g,Data_Y_g);
	Z_g    = GetData(ACCEL_ZOUT_H) + Z_g_off;
	Z_g_av = GildeAverageValueFilter(Z_g,Data_Z_g);
}
void GET_GYRO_DATA(void)
{
	X_w  = GetData(GYRO_XOUT_H) + X_w_off;
	Y_w  = GetData(GYRO_YOUT_H) + Y_w_off;
	Z_w  = GetData(GYRO_ZOUT_H) + Z_w_off;
}*/
void GET_MPU_DATA(void)
{
  GET_ACC_DATA();
	GET_GYRO_DATA();
}

