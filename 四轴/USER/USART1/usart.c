#include "usart.h"
#include <stdio.h>
#include "MPU6050.h"
#include "imu.h"
#include "control.h"
#include  "TIM3_IT.h"
#include "led.h"

void USART1_Config(void)   
{   
	GPIO_InitTypeDef   GPIO_InitStructure;   
	USART_InitTypeDef  USART_InitStructure; 
  NVIC_InitTypeDef NVIC_InitStructure;	
	/* config USART1 clock */  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);           
	/* USART1 GPIO config */  
	/* Configure USART1 Tx (PA.09) as alternate function pushpull */  
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;   
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(GPIOA, &GPIO_InitStructure);       
	/* Configure USART1 Rx (PA.10) as input floating */  
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);             
	/* USART1 mode config */  
	USART_InitStructure.USART_BaudRate 						= 115200;   
	USART_InitStructure.USART_WordLength 					= USART_WordLength_8b;   
	USART_InitStructure.USART_StopBits 						= USART_StopBits_1;   
	USART_InitStructure.USART_Parity 							= USART_Parity_No ;   
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
	USART_InitStructure.USART_Mode 								= USART_Mode_Rx | USART_Mode_Tx;   
	USART_Init(USART1, &USART_InitStructure);  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel 										= USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 3;				   //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 3;				   //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;		   //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);																		   //根据指定的参数初始化VIC寄存器
   
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART1, ENABLE);   
}
//************************************************//
//            发送字节                  				  //
//************************************************//
void usart_send(char ch)
{
	USART_SendData(USART1, (uint8_t) ch);	
  while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
}
//************************************************//
//            打印字符串			          				  //
//************************************************//
void Printt(char *p )
{
  while( *p != 0 )
	{
		usart_send(*p);
		p++;
	}
}
//************************************************//
//            发送int变量               				  //
//************************************************//
void UART_Send_int(int16_t Data)
{ 
  if(Data<0)
	{
	usart_send(0x2D);
	Data=-Data;
	}
	usart_send(Data/10000+48);	   
	Data=Data%10000;
	usart_send(Data/1000+48);
	Data=Data%1000;
	usart_send(Data/100+48);
	Data=Data%100;
	usart_send(Data/10+48);
	usart_send(Data%10+48);
}
//************************************************//
//          发送float变量               				  //
//************************************************//
void UART_Send_float( float DATA)
{
  int32_t Data;
	Data=DATA*100;
	if(Data<0)
	{
	usart_send(0x2D);
	Data=-Data;
	}	   
	usart_send(Data/100000+48);
	Data=Data%100000;
	usart_send(Data/10000+48);
	Data=Data%10000;
	usart_send(Data/1000+48);
	Data=Data%1000;
	usart_send(Data/100+48);
	Data=Data%100;
	usart_send(0x2E);
	usart_send(Data/10+48);
	usart_send(Data%10+48);
}
void USART1_IRQHandler(void)                	            //串口1中断服务程序
{
	uint8_t Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)   //接收中断
  {
	Res =USART_ReceiveData(USART1);		//读取接收到的数据
	switch(Res)
	{
		case 0x01:POWER = 1;Printt("POWER ON");break;//启动
		case 0x02:POWER = 0;Printt("POWER OFF");break;//停止
		
		case 0x03:accelerator += 5;Printt("accelerator:");UART_Send_int(accelerator);break;//加油门
		case 0x04:accelerator -= 5;Printt("accelerator:");UART_Send_int(accelerator);break;//减油门
		
		case 0x05:Vcan_send = 1;break;//发送波形
		case 0x06:Vcan_send = 0;break;//停止发送波形
		
		/*****************pitch******************/
		case 0x07:PID_PIT.oP += 0.1;Printt("pitch_oP:");UART_Send_float(PID_PIT.oP);break;
		case 0x08:PID_PIT.oP -= 0.1;Printt("pitch_oP:");UART_Send_float(PID_PIT.oP);break;
		case 0x09:PID_PIT.oI += 0.1;Printt("pitch_oI:");UART_Send_float(PID_PIT.oI);break;
		case 0x0A:PID_PIT.oI -= 0.1;Printt("pitch_oI:");UART_Send_float(PID_PIT.oI);break;
		case 0x0B:PID_PIT.oD += 0.1;Printt("pitch_oD:");UART_Send_float(PID_PIT.oD);break;
		case 0x0C:PID_PIT.oD -= 0.1;Printt("pitch_oD:");UART_Send_float(PID_PIT.oD);break;
		case 0x1B:PID_PIT.iP += 0.01;Printt("pitch_iP:");UART_Send_float(PID_PIT.iP);break;
		case 0x1C:PID_PIT.iP -= 0.01;Printt("pitch_iP:");UART_Send_float(PID_PIT.iP);break;
		case 0x1D:PID_PIT.iD += 0.2;Printt("pitch_iD:");UART_Send_float(PID_PIT.iD);break;
		case 0x1E:PID_PIT.iD -= 0.2;Printt("pitch_iD:");UART_Send_float(PID_PIT.iD);break;
			
		
		/*****************roll*****************/
		case 0x0D:PID_ROL.oP += 0.1;Printt("roll_oP:");UART_Send_float(PID_ROL.oP);break;
		case 0x0E:PID_ROL.oP -= 0.1;Printt("roll_oP:");UART_Send_float(PID_ROL.oP);break;
		case 0x0F:PID_ROL.oI += 0.1;Printt("roll_oI:");UART_Send_float(PID_ROL.oI);break;
		case 0x10:PID_ROL.oI -= 0.1;Printt("roll_oI:");UART_Send_float(PID_ROL.oI);break;
		case 0x11:PID_ROL.oD += 0.1;Printt("roll_oD:");UART_Send_float(PID_ROL.oD);break;
		case 0x12:PID_ROL.oD -= 0.1;Printt("roll_oD:");UART_Send_float(PID_ROL.oD);break;
		case 0x1F:PID_ROL.iP += 0.01;Printt("roll_iP:");UART_Send_float(PID_ROL.iP);break;
		case 0x20:PID_ROL.iP -= 0.01;Printt("roll_iP:");UART_Send_float(PID_ROL.iP);break;
		case 0x21:PID_ROL.iD += 0.2;Printt("roll_iD:");UART_Send_float(PID_ROL.iD);break;
		case 0x22:PID_ROL.iD -= 0.2;Printt("roll_iD:");UART_Send_float(PID_ROL.iD);break;
		
		
		/*****************yaw*****************/
		case 0x13:PID_YAW.oP += 0.1;Printt("yaw_oP:");UART_Send_float(PID_YAW.oP);break;
		case 0x14:PID_YAW.oP -= 0.1;Printt("yaw_oP:");UART_Send_float(PID_YAW.oP);break;
		case 0x15:PID_YAW.oD += 0.1;Printt("yaw_oD:");UART_Send_float(PID_YAW.oD);break;
		case 0x16:PID_YAW.oD -= 0.1;Printt("yaw_oD:");UART_Send_float(PID_YAW.oD);break;
		case 0x23:PID_YAW.iP += 0.1;Printt("yaw_iP:");UART_Send_float(PID_YAW.iP);break;
		case 0x24:PID_YAW.iP -= 0.1;Printt("yaw_iP:");UART_Send_float(PID_YAW.iP);break;
		case 0x25:PID_YAW.iD += 0.1;Printt("yaw_iD:");UART_Send_float(PID_YAW.iD);break;
		case 0x26:PID_YAW.iD -= 0.1;Printt("yaw_iD:");UART_Send_float(PID_YAW.iD);break;
		
		
		case 0x17:pitch_tar += 0.1;Printt("pitch_tar:");UART_Send_float(pitch_tar);break;//俯仰角
		case 0x18:pitch_tar -= 0.1;Printt("pitch_tar:");UART_Send_float(pitch_tar);break;
		
		case 0x19:roll_tar += 0.1;Printt("roll_tar:");UART_Send_float(roll_tar);break;//翻滚角
		case 0x1A:roll_tar -= 0.1;Printt("roll_tar:");UART_Send_float(roll_tar);break;
		
		//case 0x27:
		
		//case 0x34:break;//Buzzer_BiBi(2);break;
	}
	Printt("\n");
		
	//usart_send(Res);
	}
}
unsigned char Uart1_Put_Char( unsigned char Send_Data )
{
	usart_send(Send_Data);
	return Send_Data;
}
unsigned char UART1_Put_int(unsigned int Send_Data )
{
	unsigned char sum = 0;
	sum +=Uart1_Put_Char( Send_Data/256 );
	sum +=Uart1_Put_Char( Send_Data%256 );
	return sum;
}
void UART1_ReportIMU(int ax,int ay,int az,int gx,int gy,int gz,int hx,int hy,int hz,int roll,int pitch,int yaw)
{
 	unsigned char sum = 0;
	sum += Uart1_Put_Char(0x88);
	sum += Uart1_Put_Char(0xAF);
	sum += Uart1_Put_Char(0x1C);
	sum += UART1_Put_int( ax );
	sum += UART1_Put_int( ay );
	sum += UART1_Put_int( az );
	sum += UART1_Put_int( gx );
	sum += UART1_Put_int( gy );
	sum += UART1_Put_int( gz );
	sum += UART1_Put_int( hx );
	sum += UART1_Put_int( hy );
	sum += UART1_Put_int( hz );
	sum += UART1_Put_int( roll );
	sum += UART1_Put_int( pitch );
	sum += UART1_Put_int( yaw );
	sum += Uart1_Put_Char(0);
	sum += Uart1_Put_Char(0);
	sum += Uart1_Put_Char(0);
	sum += Uart1_Put_Char(0);
	Uart1_Put_Char(sum);
}

#define BUFF_NUM 10



void Vcan_Print()
{
	char Buffa[100];
	//f F \t * * 1 %d \t * * 2 %d \t * * 3 %d \t * * 4 %d E e
/*	sprintf(Buffa, "fF\t**1%d\t**2%d\t**3%d\t**4%dEe", (int)TIM4->CCR1, (int)TIM4->CCR2, (int)TIM4->CCR3,(int)TIM4->CCR4);Printt(Buffa);
	*/

	char Buff[BUFF_NUM];
	int i;
	Buff[0] = 0x03;
	Buff[1] = 0xfc;
	Buff[2] = (char)((int16_t)(pitch*10)&0x00ff);
	Buff[3] = (char)(((int16_t)(pitch*10)>>8)&0x00ff);
	Buff[4] = (char)((int16_t)(roll*10)&0x00ff);
	Buff[5] = (char)(((int16_t)(roll*10)>>8)&0x00ff);
	Buff[6] = (char)((int16_t)(yaw*10)&0x00ff);
	Buff[7] = (char)(((int16_t)(yaw*10)>>8)&0x00ff);

	Buff[BUFF_NUM-2] = 0xfc;
	Buff[BUFF_NUM-1] = 0x03;
	for(i = 0;i < BUFF_NUM; ++i)
	{
		usart_send(Buff[i]);
	}
	
	
}
