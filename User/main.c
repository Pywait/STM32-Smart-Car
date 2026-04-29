#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "usart2.h"
#include "OLED.h"
#include "hardware.h"
#include "Key.h"
#include "LED.h"
#include "Buzzer.h"
#include "PIR.h"
#include "Motor.h"
#include "Ultrasonic.h"
#include "Position.h"
#include "ISD.h"

uint8_t RxData;

int main(void)
{
	OLED_Init();
	Key_Init();
	LED_Init();	
	Buzzer_Init();
	Motor_Init();
	Ultrasonic_Init();
	Position_Init();
	ISD_Init();
	USART2_Init();
	
//	uint8_t KeyNum;				//定义用于接收按键键码的变量
//	int8_t Speed;				//定义速度变量
	
//	uint32_t Distance;
//	uint8_t PIR_Num;
	
//	uint8_t KeyNum;				//定义用于接收按键键码的变量
//	int8_t Speed;				//定义速度变量

//	uint8_t PIR_Num;
	

	while (1)
	{
		if (USART2_GetRxFlag() == 1)			//检查串口接收数据的标志位
		{
			RxData = USART2_GetRxData();		//获取串口接收的数据
			USART2_SendByte(RxData);			//串口将收到的数据回传回去，用于测试
//			OLED_ShowHexNum(1, 8, RxData, 2);	//显示串口接收的数据
		}
	}
}
