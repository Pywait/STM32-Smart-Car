#include "stm32f10x.h"                  // Device header
#include "hardware.h"
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
#include "Car.h"
#include "Track.h"
//#include "Position.h"

//uint8_t RxData;

int main(void)
{
	OLED_Init();
	LED_Init();	
	ISD_Init();
	Key_Init();
	Buzzer_Init();
	Ultrasonic_Init();
	PIR_Init();
	PWM_Init();
	Motor_Init();
	Car_Init();
	Track_Init();
//	Position_Init();
//	USART2_Init();
	
//	uint8_t KeyNum;
//	uint32_t Distance;
//	uint8_t PIR_Num;
//	int8_t Speed = 50;
	
//	OLED_ShowString(2, 1, "Distance:");
//	OLED_ShowString(2, 14, "mm");
	
	while (1)
	{
//		KeyNum = Key_GetNum();
//		if (KeyNum == 1)
//		{
//			LED_Turn();
//			Buzzer_Turn();
//		}
//		else if (KeyNum == 2)
//		{
//			uint8_t duty1 = 100;     // 左电机
//			uint8_t duty2 = 100;     // 右电机
//			Motor_L_Setspeed(duty1);
//			Motor_R_Setspeed(duty2);
//		}
//		
//		Distance = Ultrasonic_GetDistance();
//		OLED_ShowNum(2, 10, Distance, 4);
//		
//		PIR_Num = PIR_examine();
//		OLED_ShowBinNum(4, 1, PIR_Num, 2);
//		PIR_Num = PIR_examine();
//		Delay_ms(200);
//		OLED_ShowString(3, 1, "PIR_Num:");
//		OLED_ShowNum(3, 10, PIR_Num, 1);
//		
//		uint8_t code = Track_GetSensorCode();
//		OLED_ShowString(1, 1, "Code:");
//		OLED_ShowBinNum(1, 6, code, 5);
//		Delay_ms(200);
		
//		Motor_L_Setspeed(-80);

//		GPIO_SetBits(GPIOA, MOTOR_L1_A_PIN);
//		GPIO_ResetBits(GPIOA, MOTOR_L2_A_PIN);
//		PWM_SetCompare1(80);
		
		LED_OFF();

//		Car_GoForward(90);
//		Delay_s(2);
	}
}
