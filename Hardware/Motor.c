#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "PWM.h"

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = MOTOR_L1_PIN_A | MOTOR_L2_PIN_A;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MOTOR_R1_PIN_B | MOTOR_R2_PIN_B;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	PWM_Init();	
}

void Motor_L_Setspeed(uint8_t speed)
{
	if (speed > 0)
	{
		GPIO_ResetBits(GPIOA, MOTOR_L1_PIN_A);
		GPIO_SetBits(GPIOA, MOTOR_L2_PIN_A);
		PWM_SetCompare1(speed);
	}
	else if (speed < 0)
	{
		GPIO_SetBits(GPIOA, MOTOR_L1_PIN_A);
		GPIO_ResetBits(GPIOA, MOTOR_L2_PIN_A);
		PWM_SetCompare1(-speed);
	}
	else 
	{
		GPIO_SetBits(GPIOA, MOTOR_L1_PIN_A);
		GPIO_SetBits(GPIOA, MOTOR_L2_PIN_A);
		PWM_SetCompare1(0);
	}
}

void Motor_R_Setspeed(uint8_t speed)
{
	if (speed > 0)
	{
		GPIO_ResetBits(GPIOB, MOTOR_R1_PIN_B);
		GPIO_SetBits(GPIOB, MOTOR_R2_PIN_B);
		PWM_SetCompare1(speed);
	}
	else if (speed < 0)
	{
		GPIO_SetBits(GPIOB, MOTOR_R1_PIN_B);
		GPIO_ResetBits(GPIOB, MOTOR_R2_PIN_B);
		PWM_SetCompare1(-speed);
	}
	else 
	{
		GPIO_SetBits(GPIOB, MOTOR_R1_PIN_B);
		GPIO_SetBits(GPIOB, MOTOR_R2_PIN_B);
		PWM_SetCompare1(0);
	}
}
