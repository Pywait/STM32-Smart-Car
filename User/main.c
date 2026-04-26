#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "hardware.h"
#include "Key.h"
#include "LED.h"
#include "Buzzer.h"
#include "Motor.h"
#include "Ultrasonic.h"
#include "PIR.h"

int main(void)
{
	OLED_Init();
	Key_Init();
	LED_Init();	
	Buzzer_Init();
	Motor_Init();
	Ultrasonic_Init();
	
//	uint8_t KeyNum;				//定义用于接收按键键码的变量
//	int8_t Speed;				//定义速度变量
	
//	uint32_t Distance;
//	uint8_t PIR_Num;
	
//	OLED_ShowString(1, 1, "Distance:");

	while (1)
	{
		Buzzer_OFF();
//		PIR_Num = PIR_examine();
//		
//		if (PIR_Num == 1)
//		{
//			LED_ON();
//		}
//		else if (PIR_Num == 2)
//		{
//			LED_OFF();
//		}
//		else
//		{
//			Buzzer_ON();
//		}
	}
}
