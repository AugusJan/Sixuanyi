#ifndef _LED_H_  
#define _LED_H_   
  	
#include "stm32f10x.h"   
 
#define ON  0   
#define OFF 1   
 
#define LED(a) if (!a) GPIO_SetBits(GPIOA,GPIO_Pin_7);  else  GPIO_ResetBits(GPIOA,GPIO_Pin_7);
#define Buzzer(a) if (!a) GPIO_SetBits(GPIOA,GPIO_Pin_8);  else  GPIO_ResetBits(GPIOA,GPIO_Pin_8);
								
void LED_GPIO_Config(void);
void Buzzer_GPIO_Config(void);
void Buzzer_BiBi(int bi);

#endif 
