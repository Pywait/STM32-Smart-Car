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
#include "Position.h"

int main(void)
{
	OLED_Init();
	Key_Init();
	LED_Init();	
	Buzzer_Init();
	Motor_Init();
	Ultrasonic_Init();
	Position_Init();
	
//	uint8_t KeyNum;				//定义用于接收按键键码的变量
//	int8_t Speed;				//定义速度变量
	
//	uint32_t Distance;
//	uint8_t PIR_Num;
	
	while (1)
	{
//		LED_ON();
	}
}
