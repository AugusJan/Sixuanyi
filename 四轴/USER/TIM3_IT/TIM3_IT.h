#ifndef _TIM3_IT_H_
#define _TIM3_IT_H_

#include "stm32f10x.h"

extern int Vcan_send;
extern float pitch_tar;
extern float roll_tar;

void Tim3_Init(u16 period_num);//用于精确延时


#endif
