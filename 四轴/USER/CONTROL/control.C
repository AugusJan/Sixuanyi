#include "control.h"
#include "TIM4_PWM.h"


PID PID_ROL,PID_PIT,PID_YAW;

vs16 accelerator;
int  Pitch_ta;
int  Roll_ta;
int  Yaw_ta;

unsigned int  POWER;

void PID_init(void)
{
	PID_PIT.oP = 1.1;
	PID_PIT.oI = 0;
	PID_PIT.oD = 2.1;
	PID_PIT.iP = 0.2;
	//PID_PIT.iI = 0;
	PID_PIT.iD = 14.3;
	
  PID_ROL.oP = 1.1;
	PID_ROL.oI = 0;
	PID_ROL.oD = 1.2;
	PID_ROL.iP = 0.22;
	//PID_ROL.iI = 0;
	PID_ROL.iD = 12.2;

	PID_YAW.oP = 0;
	PID_YAW.oI = 0;
	PID_YAW.oD = 0;
}
float Get_MxMi(float num,float max,float min)
{
	if(num>max)
		return max;
	else if(num<min)
		return min;
	else
		return num;
}

void CONTROL(float rol_now, float pit_now, float yaw_now, float rol_tar, float pit_tar, float yaw_tar, vs16 throttle, float X_w, float Y_w, float Z_w)
{
	int moto1 = 0, moto2 = 0, moto3 = 0, moto4 = 0;
	float rol_old = 0;
	float pit_old = 0;
	float yaw_old = 0;
	float rol;
	float pit;
	float yaw;
	float X_w_old, Y_w_old, Z_w_old;
	
	PID_ROL.IMAX = throttle;	
	Get_MxMi(PID_ROL.IMAX,1000,500);
	PID_PIT.IMAX = PID_ROL.IMAX;
	
	/*外环pid*/
	rol	= rol_now - rol_tar;
	pit	= pit_now - pit_tar; 
	yaw = yaw_now - yaw_tar;
	
	PID_ROL.opout = PID_ROL.oP * rol;
	PID_PIT.opout = PID_PIT.oP * pit;
	
	if(/*rol_tar*rol_tar<0.1 && pit_tar*pit_tar<0.1 && rol_now*rol_now<30 && pit_now*pit_now<30 &&*/ throttle>660 && POWER == 1)
	{
		PID_ROL.oiout += rol;
		PID_PIT.oiout += pit;
		PID_ROL.oiout *= PID_ROL.oI;
		PID_PIT.oiout *= PID_PIT.oI;
		PID_ROL.oiout = Get_MxMi(PID_ROL.oiout,PID_ROL.IMAX,-PID_ROL.IMAX);
		PID_PIT.oiout = Get_MxMi(PID_PIT.oiout,PID_PIT.IMAX,-PID_PIT.IMAX);
	}
	else
	{
		PID_ROL.oiout = 0;
		PID_PIT.oiout = 0;
	}
	
	PID_PIT.odout = PID_PIT.oD*(pit - pit_old);
	PID_ROL.odout = PID_ROL.oD*(rol - rol_old);
	PID_YAW.odout = PID_YAW.oD*(yaw - yaw_old);

	PID_ROL.oOUT = PID_ROL.opout + PID_ROL.oiout + PID_ROL.odout;
	PID_PIT.oOUT = PID_PIT.opout + PID_PIT.oiout + PID_PIT.odout;
	PID_YAW.oOUT = PID_YAW.opout + /*PID_PIT.iout +*/ PID_YAW.odout;	
	
	/*内环pd*/
	PID_ROL.ipout = PID_ROL.iP*(PID_ROL.oOUT - X_w);
	PID_PIT.ipout = PID_PIT.iP*(PID_PIT.oOUT - Y_w);
	PID_YAW.ipout = PID_YAW.iP*(PID_YAW.oOUT - Z_w);
	
	PID_ROL.idout = PID_ROL.iD*(X_w - X_w_old);
	PID_PIT.idout = PID_PIT.iD*(Y_w - Y_w_old);
	PID_YAW.idout = PID_YAW.iD*(Z_w - Z_w_old);
	
	PID_ROL.iOUT = PID_ROL.ipout + PID_ROL.idout;
	PID_PIT.iOUT = PID_PIT.ipout + PID_PIT.idout;
	PID_YAW.iOUT = PID_YAW.ipout + PID_YAW.idout;
		
	rol_old = rol;
	pit_old = pit;
	yaw_old = yaw;
	X_w_old = X_w;
	Y_w_old = Y_w;
	Z_w_old = Z_w;
	
	moto1 = throttle + PID_ROL.iOUT - PID_PIT.iOUT + PID_YAW.iOUT;
	moto2 = throttle + PID_ROL.iOUT + PID_PIT.iOUT - PID_YAW.iOUT;
	moto3 = throttle - PID_ROL.iOUT + PID_PIT.iOUT + PID_YAW.iOUT;
	moto4 = throttle - PID_ROL.iOUT - PID_PIT.iOUT - PID_YAW.iOUT;

	if(POWER)	Moto_PwmRflash(moto1,moto2,moto3,moto4);
	else			Moto_PwmRflash(0,0,0,0);
}
//	CONTROL(roll,pitch,yaw,0,0,yaw,accelerator,X_G*Gyro_Gr,Y_G*Gyro_Gr,Z_G);	
/*
void CONTROL(float rol_now, float pit_now, float yaw_now, float rol_tar, float pit_tar, float yaw_tar, vs16 throttle, float X_w, float Y_w, float Z_w)
{
	int moto1=0,moto2=0,moto3=0,moto4=0;
	
	float rol = rol_now - rol_tar;
	float pit = pit_now - pit_tar;  
	float yaw = yaw_now - yaw_tar;
	
	PID_ROL.IMAX = throttle;	
	Get_MxMi(PID_ROL.IMAX,1000,500);
	PID_PIT.IMAX = PID_ROL.IMAX;
	
	PID_ROL.pout = PID_ROL.P * rol;
	PID_PIT.pout = PID_PIT.P * pit;
	PID_YAW.pout = PID_YAW.P * yaw;
	
	if(rol_tar*rol_tar<0.1 && pit_tar*pit_tar<0.1 && rol_now*rol_now<30 && pit_now*pit_now<30 && throttle>660 && POWER == 1)
	{
		PID_ROL.iout += PID_ROL.I * rol;
		PID_PIT.iout += PID_PIT.I * pit;
		PID_ROL.iout = Get_MxMi(PID_ROL.iout,PID_ROL.IMAX,-PID_ROL.IMAX);
		PID_PIT.iout = Get_MxMi(PID_PIT.iout,PID_PIT.IMAX,-PID_PIT.IMAX);
	}
	else
	{
		PID_ROL.iout = 0;
		PID_PIT.iout = 0;
	}
	
	PID_ROL.dout = PID_ROL.D * X_w;
	PID_PIT.dout = PID_PIT.D * Y_w;
	PID_YAW.dout = PID_YAW.D * Z_w;
		
	PID_ROL.OUT = PID_ROL.pout + PID_ROL.iout + PID_ROL.dout;
	PID_PIT.OUT = PID_PIT.pout + PID_PIT.iout + PID_PIT.dout;
	PID_YAW.OUT = PID_YAW.pout + /*PID_PIT.iout + PID_YAW.dout;	
	

	moto1 = throttle + PID_ROL.OUT - PID_PIT.OUT + PID_YAW.OUT;
	moto2 = throttle + PID_ROL.OUT + PID_PIT.OUT - PID_YAW.OUT;
	moto3 = throttle - PID_ROL.OUT + PID_PIT.OUT + PID_YAW.OUT;
	moto4 = throttle - PID_ROL.OUT - PID_PIT.OUT - PID_YAW.OUT;

	if(POWER)	Moto_PwmRflash(moto1,moto2,moto3,moto4);
	else			Moto_PwmRflash(0,0,0,0);
}*/
