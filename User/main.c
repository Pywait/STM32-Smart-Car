#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
//#include "usart2.h"
#include "LED.h"
#include "ISD.h"
#include "Key.h"
#include "Buzzer.h"
#include "Ultrasonic.h"
#include "PIR.h"
#include "Motor.h"
//#include "Position.h"
//#include "hardware.h"

//uint8_t RxData;

int main(void)
{
//	OLED_Init();
	LED_Init();	
	ISD_Init();
	Key_Init();
	Buzzer_Init();
	Ultrasonic_Init();
	PIR_Init();
	Motor_Init();
//	Position_Init();
//	USART2_Init();
	
//	uint8_t KeyNum;
//	uint32_t Distance;
//	uint8_t PIR_Num;
//	int8_t Speed = 50;
	
//	OLED_Clear();
	
	while (1)
	{
//		KeyNum = Key_GetNum();
//		if (KeyNum == 1)
//			Motor_L_Setspeed(Speed);
//		else if (KeyNum == 2)
//			Motor_R_Setspeed(Speed);
		
		Motor_L_Setspeed(20);
		Delay_s(5);
		
//		Distance = Ultrasonic_GetDistance();
		
//		PIR_Num = PIR_examine();
//		Delay_ms(200);
//		OLED_ShowString(1, 1, "PIR_Num:");
//		OLED_ShowNum(1, 10, PIR_Num, 1);
//		if (PIR_Num == 1)
//			LED_ON();
//		else if (PIR_Num == 2)
//			LED_OFF();
	}
}
