#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "Delay.h"

static uint8_t ISD_Playing = 0;

void ISD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = ISD_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ISD_PORT, &GPIO_InitStructure);
	
	GPIO_ResetBits(ISD_PORT, ISD_PIN);
	ISD_Playing = 0;
}

void ISD_PLAYE_ON(void)
{
	if (ISD_Playing == 0)
	{
		ISD_Playing = 1;
		GPIO_SetBits(ISD_PORT, ISD_PIN);
		Delay_ms(10);
		GPIO_ResetBits(ISD_PORT, ISD_PIN);
	}
}

void ISD_PLAYE_OFF(void)
{
	GPIO_ResetBits(ISD_PORT, ISD_PIN);
	ISD_Playing = 0;
}
