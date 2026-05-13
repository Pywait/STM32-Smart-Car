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
#include "Position.h"
#include "ESP.h"
#include "StateMachine.h"

// 定时启动参数 (文件作用域，会被 STOP 模式下 SRAM 保持)
static uint16_t timed_hours = 0;
static uint16_t timed_minutes = 5;
static uint8_t  timed_setting_hours = 1;  // 1=小时, 0=分钟
static uint8_t  timed_setup_entered = 0;  // 判断 STATE_TIMED_SETUP 是否首次进入
static uint32_t timed_last_activity = 0;  // 最后一次按键时间戳

/* ── RTC 定时启动（STOP 模式） ─────────────────── */

static void RTC_EnterTimedStartup(uint16_t minutes)
{
	// 使能 PWR / BKP 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);

	// LSI → RTC 时钟
	RCC_LSICmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();

	// 预分频：LSI ≈ 40 kHz → 1 Hz
	RTC_SetPrescaler(40000 - 1);
	RTC_WaitForLastTask();

	// 相对闹钟：计数器从 0 开始，闹钟值为 minutes × 60
	RTC_SetCounter(0);
	RTC_WaitForLastTask();
	RTC_SetAlarm(minutes * 60);
	RTC_WaitForLastTask();

	// EXTI 线 17：RTC 闹钟唤醒 STOP 模式
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// RTC 闹钟中断
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

	RTC_ITConfig(RTC_IT_ALR, ENABLE);
	RTC_WaitForLastTask();

	// 显示并进入 STOP 模式
	OLED_Clear();
	OLED_ShowString(1, 1, "TIMED START");
	OLED_ShowString(2, 1, "Sleeping...");
	Delay_ms(300);

	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

	// ========== RTC 闹钟唤醒 ==========
	SystemInit();   // 恢复 HSE + PLL 时钟

	// 清除闹钟标志
	if (RTC_GetFlagStatus(RTC_FLAG_ALR) != RESET) {
		RTC_ClearFlag(RTC_FLAG_ALR);
		RTC_WaitForLastTask();
	}

	OLED_Clear();
	OLED_ShowString(1, 1, "TIME'S UP!");
	Delay_ms(300);
}

/* ── 主函数 ────────────────────────────────────── */

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
	Position_Init();
	ESP_Init();

	StateMachine_Init();

	// MQTT 连接
	OLED_ShowString(1, 1, "MQTT connecting...");
	ESP_MQTTInit();
	OLED_Clear();
	OLED_ShowString(1, 1, "MQTT OK");

	uint8_t KeyNum;

	while (1)
	{
		KeyNum = Key_GetNum();

		/* ── 状态转移（按键驱动） ───────────────── */
		switch (current_state)
		{
		case STATE_STANDBY:
			if (KeyNum == 1) {
				OLED_Clear();
				current_state = STATE_MODE_SELECT;
			}
			break;

		case STATE_MODE_SELECT:
			if (KeyNum == 1) {
				current_mode = MODE_TRACKING;
				current_state = STATE_STARTUP_SELECT;
			} else if (KeyNum == 2) {
				current_mode = MODE_OBSTACLE_AVOIDANCE;
				current_state = STATE_STARTUP_SELECT;
			}
			break;

		case STATE_STARTUP_SELECT:
			if (KeyNum == 1) {
				// 立即启动
				OLED_Clear();
				if (current_mode == MODE_TRACKING) {
					current_state = STATE_TRACKING;
					OLED_ShowString(1, 1, "TRACKING MODE");
				} else {
					current_state = STATE_OBSTACLE_AVOIDANCE;
					OLED_ShowString(1, 1, "OBSTACLE MODE");
				}
			} else if (KeyNum == 2) {
				// 定时启动
				timed_hours = 0;
				timed_minutes = 5;
				timed_setting_hours = 1;
				timed_setup_entered = 0;
				current_state = STATE_TIMED_SETUP;
			}
			break;

		case STATE_TIMED_SETUP:
			if (!timed_setup_entered) {
				timed_setup_entered = 1;
				timed_last_activity = Delay_GetTick();
			}

			if (KeyNum == 1) {
				timed_setting_hours = !timed_setting_hours;
				timed_last_activity = Delay_GetTick();
			} else if (KeyNum == 2) {
				if (timed_setting_hours)
					timed_hours = (timed_hours + 1) % 24;
				else
					timed_minutes = (timed_minutes + 1) % 60;
				timed_last_activity = Delay_GetTick();
			}

			OLED_Clear();
			OLED_ShowString(1, 1, "TIMED SETUP");
			OLED_ShowString(2, 1, "H:  ");
			OLED_ShowNum(2, 4, timed_hours, 2);
			OLED_ShowString(2, 9, "M:  ");
			OLED_ShowNum(2, 12, timed_minutes, 2);
			OLED_ShowString(3, 1, "KEY1: switch");
			OLED_ShowString(4, 1, "KEY2: +1");
			if (timed_setting_hours)
				OLED_ShowString(2, 14, "H");
			else
				OLED_ShowString(2, 14, "M");

			// 3 秒无按键 → 自动确认，进入定时休眠
			if (Delay_GetTick() - timed_last_activity > 3000) {
				uint16_t total_min = timed_hours * 60 + timed_minutes;
				if (total_min < 1) total_min = 1;
				if (total_min > 1440) total_min = 1440;

				// 休眠前先设为目标状态
				if (current_mode == MODE_TRACKING)
					current_state = STATE_TRACKING;
				else
					current_state = STATE_OBSTACLE_AVOIDANCE;

				RTC_EnterTimedStartup(total_min);

				// 唤醒后继续
				OLED_Clear();
				if (current_mode == MODE_TRACKING) {
					current_state = STATE_TRACKING;
					OLED_ShowString(1, 1, "TRACKING MODE");
				} else {
					current_state = STATE_OBSTACLE_AVOIDANCE;
					OLED_ShowString(1, 1, "OBSTACLE MODE");
				}
			}
			break;

		default:
			break;
		}

		StateMachine_Update();
		Ultrasonic_Update();
		ESP_ProcessCommands();

		Delay_ms(10);
	}
}
