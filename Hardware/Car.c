#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "Motor.h"

void Motor_SelfLight(int8_t Lspeed, int8_t Rspeed)
{
	
	PWM_SetCompare1(Lspeed);
	PWM_SetCompare1(-Rspeed);
}

void Car_Forward(uint8_t speed)
{
	Motor_L_Setspeed(speed);
	Motor_R_Setspeed(speed);
}

void Car_Backward(uint8_t speed)
{
	Motor_L_Setspeed(-speed);
	Motor_R_Setspeed(-speed);
}
void Car_TurnLeft(uint8_t speed_diff, uint8_t base_speed);   // 左转幅度可调
void Car_TurnRight(uint8_t speed_diff, uint8_t base_speed);

void Car_SpinLeft(uint8_t speed)
{
	Motor_L_Setspeed(-speed);
	Motor_R_Setspeed(+speed);
}

void Car_SpinRight(uint8_t speed)
{
	Motor_L_Setspeed(+speed);
	Motor_R_Setspeed(-speed);
}
