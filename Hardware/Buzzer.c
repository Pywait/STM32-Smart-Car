#include "stm32f10x.h"                  // Device header
#include "hardware.h"

void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
	
	GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
}

void Buzzer_ON(void)
{
	GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
}

void Buzzer_OFF(void)
{
	GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
}

void Buzzer_Turn(void)
{
	if (GPIO_ReadOutputDataBit(BUZZER_PORT, BUZZER_PIN) == 1)
	{
		GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
	}
	else
	{
		GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
	}
}
