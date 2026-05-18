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

/**
  * @brief  被动红外检测。
  * @retval 0=无人, 1=前方有人, 2=后方有人。
  */
uint8_t PIR_examine(void)
{
	uint8_t PIR_examine_Num = 0;

	if (GPIO_ReadInputDataBit(PIR_PORT, PIR_FRONT_PIN) == 1)
	{
		Delay_us(10);
		if (GPIO_ReadInputDataBit(PIR_PORT, PIR_FRONT_PIN) == 1)
			PIR_examine_Num = 1;
	}
	if (GPIO_ReadInputDataBit(PIR_PORT, PIR_BACK_PIN) == 1)
	{
		Delay_us(10);
		if (GPIO_ReadInputDataBit(PIR_PORT, PIR_BACK_PIN) == 1)
			PIR_examine_Num = 2;
	}

	return PIR_examine_Num;
}
