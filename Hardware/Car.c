#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "Motor.h"

void Car_Init(void)
{
	Motor_Init();
}

void Car_GoForward(uint8_t speed)
{
	if (speed > 100) speed = 100;
	Motor_L_Setspeed(speed);
	Motor_R_Setspeed(speed);
}

void Car_GoBackward(uint8_t speed)
{
	if (speed > 100) speed = 100;
	Motor_L_Setspeed(-(int8_t)speed);
	Motor_R_Setspeed(-(int8_t)speed);
}

/* 非麦克纳姆轮，暂不使用
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
*/
void Car_TurnLeft(uint8_t speed)
{
    if (speed > 100) speed = 100;
    if (speed < 75) speed = 75;

    int8_t left = (int8_t)speed;                     // 左轮保持不变
    int8_t right = (int8_t)speed + (100 - speed) / 2; // 右轮加速
    if (right > 100) right = 100;

    Motor_L_Setspeed(left);
    Motor_R_Setspeed(right);
}

void Car_TurnRight(uint8_t speed)
{
    if (speed > 100) speed = 100;
    if (speed < 75) speed = 75;

    int8_t left = (int8_t)speed + (100 - speed) / 2; // 左轮加速
    if (left > 100) left = 100;
    int8_t right = (int8_t)speed;                    // 右轮保持不变

    Motor_L_Setspeed(left);
    Motor_R_Setspeed(right);
}

void Car_Brake(void)
{
	Motor_Brake();
}

void Car_Stop(void)
{
	Motor_Stop();
}
