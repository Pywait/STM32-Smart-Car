#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "Delay.h"

/** 反射式红外传感器：
  * 白色背景，接收成功，开光提示灯亮，输出0；
  * 黑色线条，未接收，开光提示灯灭，输出1。
  */

static uint8_t lastState = 0;			//全局静态变量
static uint16_t Position0 = 0;
static uint16_t Position = 0;

void Position_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = POSITION_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(POSITION_PORT, &GPIO_InitStructure);
}

uint16_t Position_Get(uint16_t TPNum)
{
	if (lastState == 0 && GPIO_ReadInputDataBit(POSITION_PORT, POSITION_PIN) == 1)
	{
		Delay_us(20);
		if (GPIO_ReadInputDataBit(POSITION_PORT, POSITION_PIN) == 1)
		{
			Position0 ++;
			lastState = 1;
		}
	}
	if (lastState == 1 && GPIO_ReadInputDataBit(POSITION_PORT, POSITION_PIN) == 0)
	{
		if (GPIO_ReadInputDataBit(POSITION_PORT, POSITION_PIN) ==1)
		{
			lastState = 0;
		}
	}
	Position = Position0 % TPNum;
	return Position;
}
