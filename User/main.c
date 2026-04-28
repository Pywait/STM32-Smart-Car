#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "usart2.h"
#include "hardware.h"
#include "Key.h"
#include "LED.h"
#include "Buzzer.h"
#include "Motor.h"
#include "Ultrasonic.h"
#include "PIR.h"
#include "Position.h"
#include "ISD.h"

uint8_t RxData;

int main(void)
{
	USART2_Init();
	OLED_Init();
	Key_Init();
	LED_Init();	
	Buzzer_Init();
	Motor_Init();
	Ultrasonic_Init();
	Position_Init();
	ISD_Init();
	
	uint8_t KeyNum;				//定义用于接收按键键码的变量
//	int8_t Speed;				//定义速度变量
	
//	uint32_t Distance;
//	uint8_t PIR_Num;
	
//	uint8_t KeyNum;				//定义用于接收按键键码的变量
//	int8_t Speed;				//定义速度变量

//	uint8_t PIR_Num;
	

	while (1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 1)
			ISD_PLAYL_ON();
		else if (KeyNum == 2)
			ISD_PLAYL_OFF();
	}
}
