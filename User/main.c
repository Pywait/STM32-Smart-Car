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
#include "PWM.h"
#include "Motor.h"
#include "Track.h"
//#include "Position.h"
//#include "hardware.h"

//uint8_t RxData;

int main(void)
{
	OLED_Init();
	LED_Init();	
	ISD_Init();
//	Key_Init();
	Buzzer_Init();
	Ultrasonic_Init();
	PIR_Init();
	PWM_Init();
	Motor_Init();
	Track_Init();
//	Position_Init();
//	USART2_Init();
	
//	uint8_t KeyNum;
//	uint32_t Distance;
//	uint8_t PIR_Num;
//	int8_t Speed = 50;
	
	while (1)
	{
//		KeyNum = Key_GetNum();
//		if (KeyNum == 1)
//			Motor_L_Setspeed(Speed);
//		else if (KeyNum == 2)
//			Motor_R_Setspeed(Speed);
//		Distance = Ultrasonic_GetDistance();
		
		uint8_t duty1 = 80;     // 左电机
		uint8_t duty2 = 90;     // 右电机
		Motor_L_Setspeed(duty1);
		Motor_R_Setspeed(duty2);
		
//		uint8_t code = Track_GetSensorCode();
//		OLED_ShowString(1, 1, "Code:");
//		OLED_ShowBinNum(2, 1, code, 5);
//		Delay_ms(200);

	}
}
