#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "hardware.h"
#include "Key.h"
#include "LED.h"

#include "Buzzer.h"
#include "Motor.h"

uint8_t Key_Num;

int main(void)
{
	Key_Init();
	LED_Init();
	
	Buzzer_Init();
	Motor_Init();
	
	uint8_t KeyNum;		//定义用于接收按键键码的变量
	int8_t Speed;		//定义速度变量

	
	while(1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 1)
		{
			Speed += 20;
			if (Speed > 80)
			{
				Speed = 0;
			}
		}
		Motor_R_Setspeed(Speed);
	}
}
