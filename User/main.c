#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"

int main(void)
{
	OLED_Init();
	
	uint8_t Echo;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	
	while(1)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
		Delay_us(20);
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		Echo = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);
		OLED_ShowString(2, 1, "Echo:");
		OLED_ShowNum(2, 5, Echo, 1);
		while (Echo == 1)
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_0);
			Echo = 0;
			break;
		}
		Delay_ms(200);
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
	}
}
