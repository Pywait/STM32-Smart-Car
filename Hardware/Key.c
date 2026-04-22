#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "hardware.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_PIN1 | KEY_PIN2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	
	if (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN1) == 0)
	{
		Delay_ms(20);
		if (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN1) == 0)
		{
			while (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN1) == 0);
			Delay_ms(20);
			KeyNum = 1;
		}
	}
	
	if (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN2) == 0)
	{
		Delay_ms(20);
		if (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN2) == 0)
		{
			while (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN2) == 0);
			Delay_ms(20);
			KeyNum = 2;
		}
	}
	
	return KeyNum;
}
