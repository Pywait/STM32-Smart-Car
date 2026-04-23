#include "stm32f10x.h"                  // Device header
#include "hardware.h"

void Track_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = TRACK_L1_PIN | TRACK_L2_PIN | TRACK_R1_PIN | TRACK_R2_PIN | TRACK_M_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRACK_PORT, &GPIO_InitStructure);
}

uint16_t Track_GetTrackNum(void)
{
	uint16_t TrackNum;
	
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_L1_PIN) == 0)
		TrackNum = 1;
	else if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_L2_PIN) == 0)
		TrackNum = 2;
	else if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_R1_PIN) == 0)
		TrackNum = 3;
	else if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_R2_PIN) == 0)
		TrackNum = 4;
	else if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_M_PIN) == 0)
		TrackNum = 5;

	return TrackNum;
}

void Track_Track(void)
{
	
}
