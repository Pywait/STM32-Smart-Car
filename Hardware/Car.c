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
    Motor_L_Setspeed(speed - 12);   // 左轮半速，可调整比例
    Motor_R_Setspeed(speed);
}

void Car_TurnRight(uint8_t speed)
{
    if (speed > 99) speed = 99;
    Motor_L_Setspeed(speed);
    Motor_R_Setspeed(speed - 12);
}

void Car_PID_TurnLeft(uint8_t speed_diff, uint8_t base_speed)
{
	if (base_speed > 99) base_speed = 99;
	if (speed_diff > base_speed) speed_diff = base_speed;
	int8_t l = (int8_t)base_speed - (int8_t)speed_diff;
	if (l < 0) l = 0;
	Motor_L_Setspeed(l);
	Motor_R_Setspeed(base_speed);
}

void Car_PID_TurnRight(uint8_t speed_diff, uint8_t base_speed)
{
	if (base_speed > 99) base_speed = 99;
	if (speed_diff > base_speed) speed_diff = base_speed;
	int8_t r = (int8_t)base_speed - (int8_t)speed_diff;
	if (r < 0) r = 0;
	Motor_L_Setspeed(base_speed);
	Motor_R_Setspeed(r);
}

void Car_Brake(void)
{
	Motor_Brake();
}

void Car_Stop(void)
{
	Motor_Stop();
}
