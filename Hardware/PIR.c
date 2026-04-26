#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "Delay.h"

void PIR_Init(void)
{
	RCC_APB2PeriphClockCmd(PIR_RCC_PORT, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//下拉输入
	GPIO_InitStructure.GPIO_Pin = PIR_FRONT_PIN | PIR_BACK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PIR_PORT, &GPIO_InitStructure);
	
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12 | GPIO_PinSource13);		//人体检测功能不需要中断
//	
//	EXTI_InitTypeDef EXTI_InitStructure;
//	EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line13;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_Init(&EXTI_InitStructure);
//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//	NVIC_Init(&NVIC_InitStructure);
}

/**
	@brif 被动红外检测。
	@retval 检测结果，uint16_t，1前方有人，2后方有人。
	*/
uint16_t PIR_examine(void)
{
	uint16_t PIR_examine_Num = 0;
	
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
