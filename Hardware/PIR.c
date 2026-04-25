#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "Delay.h"

void PIR_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = PIR_FRONT_PIN | PIR_BACK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PIR_PORT, &GPIO_InitStructure);
}
