#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "Delay.h"
#include "OLED.h"
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
#include "ESP.h"
#include "StateMachine.h"

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
	ESP_Init();
	
	StateMachine_Init();
	
	uint8_t KeyNum;
	
	while (1)
	{
		KeyNum = Key_GetNum();
		
		if (KeyNum == 1) {
			if (current_state == STATE_STANDBY) {
				StateMachine_SetState(STATE_MODE_SELECT);
			} else if (current_state == STATE_MODE_SELECT) {
				StateMachine_SetMode(MODE_TRACKING);
				StateMachine_SetState(STATE_TRACKING);
				OLED_Clear();
				OLED_ShowString(1, 1, "TRACKING MODE");
				Delay_ms(500);
			}
		}
		
		if (KeyNum == 2) {
			if (current_state == STATE_MODE_SELECT) {
				StateMachine_SetMode(MODE_OBSTACLE_AVOIDANCE);
				StateMachine_SetState(STATE_OBSTACLE_AVOIDANCE);
				OLED_Clear();
				OLED_ShowString(1, 1, "OBSTACLE MODE");
				Delay_ms(500);
			}
		}
		
		// KeyNum == 3 预留（第三个按键停止）
		// if (KeyNum == 3) {
		//     StateMachine_SetState(STATE_STANDBY);
		//     OLED_Clear();
		//     LED_OFF();
		//     Buzzer_OFF();
		//     Car_Stop();
		// }
		
		StateMachine_Update();
		ESP_ProcessCommands();
		
		Delay_ms(10);
	}
}
