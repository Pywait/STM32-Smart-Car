#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "hardware.h"
#include "Key.h"
#include "LED.h"
#include "Buzzer.h"

uint8_t Key_Num;

int main(void)
{
	Key_Init();
	LED_Init();
	
	Buzzer_Init();
	
	uint8_t KeyNum = 0;
		
	while(1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 1)
		{
			LED_Turn();
		}
		else if (KeyNum == 2)
		{
			Buzzer_Turn();
		}
	}
}
