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
	
	uint8_t KeyNum;
//	uint32_t Distance;
//	uint8_t PIR_Num;
//	int8_t Speed = 50;
	
	
	while (1)
	{
//		LED_Turn();
//		Buzzer_Turn();
//		ISD_PLAYL_ON();
//		uint8_t code = Track_GetSensorCode();
//		OLED_ShowString(1, 1, "Code:");
//		OLED_ShowBinNum(1, 6, code, 5);
//		OLED_ShowString(2, 1, "Distance:");
//		OLED_ShowString(2, 14, "mm");
//		Distance = Ultrasonic_GetDistance();
//		OLED_ShowNum(2, 10, Distance, 4);
//		PIR_Num = PIR_examine();
//		OLED_ShowBinNum(3, 1, PIR_Num, 2);
//		PIR_Num = PIR_examine();
//		OLED_ShowString(3, 4, "PIR_Num:");
//		OLED_ShowNum(3, 13, PIR_Num, 1);
		
		
		KeyNum = Key_GetNum();
		if (KeyNum == 1)		{}
		else if (KeyNum == 2)		{}
//		
//		Motor_L_Setspeed(80);
//		Delay_s(3);
//		Motor_Stop();

//		GPIO_ResetBits(GPIOA, MOTOR_L1_A_PIN);
//		GPIO_SetBits(GPIOA, MOTOR_L2_A_PIN);
//		PWM_SetCompare1(80);
//		GPIO_SetBits(GPIOB, MOTOR_R1_B_PIN);
//		GPIO_ResetBits(GPIOB, MOTOR_R2_B_PIN);
//		PWM_SetCompare2(80);

		GPIO_ResetBits(GPIOA, MOTOR_L1_A_PIN);
		GPIO_SetBits(GPIOA, MOTOR_L2_A_PIN);
		PWM_SetCompare1(100);
		GPIO_ResetBits(GPIOB, MOTOR_R1_B_PIN);
		GPIO_SetBits(GPIOB, MOTOR_R2_B_PIN);
		PWM_SetCompare2(100);
			
//		Car_GoForward(90);
//		Delay_s(2);
	}
}
