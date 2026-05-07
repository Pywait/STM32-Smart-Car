#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "PWM.h"

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = MOTOR_L1_A_PIN | MOTOR_L2_A_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MOTOR_R1_B_PIN | MOTOR_R2_B_PIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	PWM_Init();	
}

void Motor_L_Setspeed(int8_t speed)
{
	if (speed > 0)
	{
		GPIO_SetBits(GPIOA, MOTOR_L1_A_PIN);
		GPIO_ResetBits(GPIOA, MOTOR_L2_A_PIN);
		PWM_SetCompare1(speed);
	}
	else if (speed < 0)
	{
		GPIO_ResetBits(GPIOA, MOTOR_L1_A_PIN);
		GPIO_SetBits(GPIOA, MOTOR_L2_A_PIN);
		PWM_SetCompare1(-speed);
	}
	else
	{
		GPIO_SetBits(GPIOA, MOTOR_L1_A_PIN);
		GPIO_SetBits(GPIOA, MOTOR_L2_A_PIN);
		PWM_SetCompare1(0);
	}
}

void Motor_R_Setspeed(int8_t speed)
{
	if (speed > 0)
	{
		GPIO_SetBits(GPIOB, MOTOR_R1_B_PIN);
		GPIO_ResetBits(GPIOB, MOTOR_R2_B_PIN);
		PWM_SetCompare2(speed);
	}
	else if (speed < 0)
	{
		GPIO_ResetBits(GPIOB, MOTOR_R1_B_PIN);
		GPIO_SetBits(GPIOB, MOTOR_R2_B_PIN);
		PWM_SetCompare2(-speed);
	}
	else
	{
		GPIO_SetBits(GPIOB, MOTOR_R1_B_PIN);
		GPIO_SetBits(GPIOB, MOTOR_R2_B_PIN);
		PWM_SetCompare2(0);
	}
}

void Motor_Brake(void)		// 快速刹车
{
	GPIO_SetBits(GPIOA, MOTOR_L1_A_PIN | MOTOR_L2_A_PIN);
	GPIO_SetBits(GPIOB, MOTOR_R1_B_PIN | MOTOR_R2_B_PIN);
	PWM_SetCompare1(100);
	PWM_SetCompare2(100);
}
void Motor_Stop(void)			// 自由停止
{
	PWM_SetCompare1(0);
	PWM_SetCompare2(0);
}
