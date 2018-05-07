#include  "TIM3_IT.h"
#include  "imu.h"
#include  "mpu6050.h"
#include  "control.h"
#include  "NRF24L01.h"
#include  "usart.h"
#include  "adc.h"

#include "IIC.h"

int Vcan_send = 1;
float pitch_tar = 0;
float roll_tar = 0;

/**************************ʵ�ֺ���********************************************
*����ԭ��:		
*��������:1ms�ж�һ��,������Ϊ1000		
*******************************************************************************/
void Tim3_Init(u16 period_num)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_DeInit(TIM3);

	TIM_TimeBaseStructure.TIM_Period        = period_num;   //װ��ֵ	//prescaler is 1200,that is 72000000/72/500=2000Hz;
	TIM_TimeBaseStructure.TIM_Prescaler     = 72-1;         //��Ƶϵ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //or TIM_CKD_DIV2 or TIM_CKD_DIV4
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);	
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );   //ʹ��ָ����TIM3�ж�,��������ж�
	
	//�ж����ȼ�NVIC����	
	NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;          //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;          //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;     //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	
	TIM_Cmd(TIM3, DISABLE);  //ʹ��TIMx		
}

void TME_5ms(void)
{
	static int flag = 0;
	GET_MPU_DATA();
	Attitude_UpdateGyro();                /////????
  Attitude_UpdateAcc();                 /////??????
	pitch = EulerAngle.Roll / PI * 180;            ////????   ???
  roll = EulerAngle.Pitch / PI * 180;             ////???
  yaw = EulerAngle.Yaw / PI * 180;                ////???
	
	
  //IMUupdate(X_G*Gyro_Gr,Y_G*Gyro_Gr,Z_G*Gyro_Gr,X_A_av,Y_A_av,Z_A_av);
	if(roll > 45 || pitch >45 || roll < -45 || pitch < -45)
	{
		POWER = 0;
		flag = 1;
	}
	if(flag)
	{
		POWER = 0;
	}
	//CONTROL(roll,pitch,yaw,roll_tar,pitch_tar,0,accelerator, X_G*Gyro_Gr,Y_G*Gyro_Gr,Z_G*Gyro_Gr);	
}

void TIM3_IRQHandler(void)		
{	
	static u8 ms10 = 0;				
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)    //���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);         //���TIMx�����жϱ�־ 

		TME_5ms();
		++ms10;
		if(ms10 == 2 && Vcan_send)
		{
			Vcan_Print();
			ms10 = 0;
		}

	}
}

//  UART1_ReportIMU(X_g_av,Y_g_av,Z_g_av,X_w,Y_w,Z_w,RX_speed,TX_speed,VCC*10,roll*100,pitch*100,yaw*10);	


