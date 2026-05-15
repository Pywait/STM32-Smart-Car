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
	if (speed > 99) speed = 99;
	Motor_L_Setspeed(-(int8_t)speed);
	Motor_R_Setspeed(-(int8_t)speed);
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
    int8_t left = (int8_t)speed - 12;
    if (left < 0) left = 0;
    Motor_L_Setspeed(left);         // 左轮减速 → 左转
    Motor_R_Setspeed((int8_t)speed);
}

void Car_TurnRight(uint8_t speed)
{
    if (speed > 99) speed = 99;
    Motor_L_Setspeed((int8_t)speed);
    int8_t right = (int8_t)speed - 12;
    if (right < 0) right = 0;
    Motor_R_Setspeed(right);        // 右轮减速 → 右转
}

void Car_Brake(void)
{
	Motor_Brake();
}

void Car_Stop(void)
{
	Motor_Stop();
}
