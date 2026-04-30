#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
//#include "usart2.h"
#include "LED.h"
#include "ISD.h"
#include "Key.h"
//#include "hardware.h"
//#include "Buzzer.h"
//#include "PIR.h"
//#include "Motor.h"
//#include "Ultrasonic.h"
//#include "Position.h"

//uint8_t RxData;

int main(void)
{
//	OLED_Init();
	LED_Init();	
	ISD_Init();
	Key_Init();
//	Buzzer_Init();
//	Motor_Init();
//	Ultrasonic_Init();
//	Position_Init();
//	USART2_Init();
	
	uint8_t KeyNum;				//定义用于接收按键键码的变量
//	int8_t Speed;				//定义速度变量
//	uint32_t Distance;
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
