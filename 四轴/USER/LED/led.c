#include "led.h"

void LED_GPIO_Config(void)   
{       
		//PA7 LED
	GPIO_InitTypeDef GPIO_InitStructureLED;
    
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	
    GPIO_InitStructureLED.GPIO_Pin   = GPIO_Pin_7  ;          
    GPIO_InitStructureLED.GPIO_Mode  = GPIO_Mode_Out_PP;             
    GPIO_InitStructureLED.GPIO_Speed = GPIO_Speed_10MHz;  
	
    GPIO_Init(GPIOA, &GPIO_InitStructureLED);     
		
		
		LED(OFF);
}

void Buzzer_GPIO_Config(void)
{
	//PA8 Buzzer
    GPIO_InitTypeDef GPIO_InitStructureBuz;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	
    GPIO_InitStructureBuz.GPIO_Pin   = GPIO_Pin_8;          
    GPIO_InitStructureBuz.GPIO_Mode  = GPIO_Mode_Out_PP;             
    GPIO_InitStructureBuz.GPIO_Speed = GPIO_Speed_10MHz;  
	
    GPIO_Init(GPIOA, &GPIO_InitStructureBuz); 
	
	Buzzer(OFF);
}

void Buzzer_BiBi(int bi)
{
	int i;
	
	for(i = 0;i < bi;i++)
	{
		Buzzer(ON);
		Delay_us(100000);
		Buzzer(OFF);
		Delay_us(100000);
	}
}
