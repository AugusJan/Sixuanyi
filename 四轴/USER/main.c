#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "SysTick.h"
#include "TIM3_IT.h"
#include "led.h"
#include "adc.h"
#include "usart.h"
#include "IIC.h"
#include "MPU6050.h"
#include "TIM4_PWM.h"
#include "SPI_IO.h"
#include "NRF24L01.h"
#include "control.h"

void Init_All()
{
	int i;
	SysTick_Init();
	
	LED_GPIO_Config();
	Buzzer_GPIO_Config();
	Buzzer_BiBi(2);
	
	InitMPU6050();
	Quaternion_init();
	GET_MPU_DATA();	
	
	USART1_Config();
	Tim3_Init(5000);
	 
	TIM4_PWM_Init();
	PID_init();
	Printt("IInit Done!\n");
	sleep(3);
	LED(ON);
	Buzzer_BiBi(6);
}

int main()
{
	/********************Variable Defined******************************/

	
	/********************Init Module***********************************/
	Init_All();
	TIM_Cmd(TIM3, ENABLE);
	/********************Main******************************************/
	accelerator = 650;
	//Delay_us(200000);
	LED(OFF);
	//POWER = 1;
	while(1)
	{
	
	}
}
