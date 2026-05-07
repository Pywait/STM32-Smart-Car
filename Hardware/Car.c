#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "Motor.h"

void Car_Init(void)
{
	Motor_Init();
}

void Car_GoForward(uint8_t speed)
{
	if (speed > 99) speed = 99;
	Motor_L_Setspeed(speed);
	Motor_R_Setspeed(speed);
}

void Car_GoBackward(uint8_t speed)
{
	if (-speed > 99) speed = 99;
	Motor_L_Setspeed(-speed);
	Motor_R_Setspeed(-speed);
}

void Car_SpinLeft(uint8_t speed)
{
	if (speed > 99) speed = 99;
	Motor_L_Setspeed(-speed);
	Motor_R_Setspeed(+speed);
}

void Car_SpinRight(uint8_t speed)
{
	if (speed > 99) speed = 99;
	Motor_L_Setspeed(+speed);
	Motor_R_Setspeed(-speed);
}

void Car_TurnLeft(uint8_t speed)
{
    if (speed > 99) speed = 99;
    Motor_L_Setspeed(speed - 12);   // 左轮半速，可调整比例
    Motor_R_Setspeed(speed);
}

void Car_TurnRight(uint8_t speed)
{
    if (speed > 99) speed = 99;
    Motor_L_Setspeed(speed);
    Motor_R_Setspeed(speed - 12);
}

void Car_PID_TurnLeft(uint8_t speed_diff, uint8_t base_speed);   // 左转幅度可调
void Car_PID_TurnRight(uint8_t speed_diff, uint8_t base_speed);

void Car_Brake(void)
{
	Motor_Brake();
}

void Car_Stop(void)
{
	Motor_Stop();
}
