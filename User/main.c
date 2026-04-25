#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "hardware.h"
#include "Key.h"
#include "LED.h"
#include "Buzzer.h"
#include "Motor.h"
#include "Ultrasonic.h"

uint8_t Key_Num;

int main(void)
{
	OLED_Init();
	Key_Init();
	LED_Init();	
	Buzzer_Init();
	Motor_Init();
	Ultrasonic_Init();
	
	uint8_t KeyNum;				//定义用于接收按键键码的变量
	int8_t Speed;				//定义速度变量
	
	uint32_t Distance;

	OLED_ShowString(1, 1, "Distance:");

	while (1)
	{
		Distance = Ultrasonic_GetDistance();

		OLED_ShowNum(2, 1, Distance, 5);
		OLED_ShowString(2, 6, "mm");
	}
}
