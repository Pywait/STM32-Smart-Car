#include "StateMachine.h"
#include "hardware.h"
#include "Delay.h"
#include "LED.h"
#include "Buzzer.h"
#include "ISD.h"
#include "ESP.h"
#include "Car.h"
#include "Ultrasonic.h"
#include "PIR.h"
#include "Track.h"
#include "Position.h"
#include "Motor.h"
#include "OLED.h"

#define POSITION_MARKERS 10   // 标志点总数，根据实际轨道修改

SystemState current_state = STATE_STANDBY;
OperationMode current_mode = MODE_TRACKING;
uint8_t alarm_active = 0;

static uint8_t human_detected = 0;
static char last_remote_cmd = 'S';

void RemoteCtrl_SetCmd(char cmd) { last_remote_cmd = cmd; }

void StateMachine_Init(void)
{
	current_state = STATE_STANDBY;
	current_mode = MODE_TRACKING;
	alarm_active = 0;
	human_detected = 0;
}

void StateMachine_SetState(SystemState state)
{
	current_state = state;
}

void StateMachine_SetMode(OperationMode mode)
{
	current_mode = mode;
}

void TriggerObstacleAlarm(void)
{
	if (alarm_active) return;

	alarm_active = 1;

	LED_ON();
	Buzzer_ON();
	ESP_SendAlarm(0x02);

	OLED_Clear();
	OLED_ShowString(1, 1, "OBSTACLE!");
	OLED_ShowString(2, 1, "STOPPED");
}

void TriggerHumanAlarm(void)
{
	if (alarm_active) return;

	alarm_active = 1;
	human_detected = 1;

	LED_ON();
	Buzzer_ON();
	ISD_PLAYL_ON();
	ESP_SendAlarm(0x01);

	OLED_Clear();
	OLED_ShowString(1, 1, "HUMAN DETECTED!");
	OLED_ShowString(2, 1, "ALARM!");
}

void ClearAlarms(void)
{
	alarm_active = 0;
	human_detected = 0;
	LED_OFF();
	Buzzer_OFF();
	ISD_PLAYL_OFF();
}

/* ── 各状态处理 ────────────────────────────────── */

static void StandbyMode(void)
{
	OLED_ShowString(1, 1, "STANDBY");
	OLED_ShowString(2, 1, "Press KEY1 to start");
	Car_Stop();
	LED_OFF();
	Buzzer_OFF();
	Delay_ms(200);

	/* ── 保存超声波的 EXTI11 配置 ── */
	uint32_t save_exticr2 = AFIO->EXTICR[2];
	uint32_t save_imr = EXTI->IMR;
	uint32_t save_emr = EXTI->EMR;
	uint32_t save_rtsr = EXTI->RTSR;
	uint32_t save_ftsr = EXTI->FTSR;

	/* ── EXTI11 事件模式：PA11(KEY1)下降沿唤醒 ── */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
	EXTI->IMR  &= ~EXTI_Line11;       // 关中断
	EXTI->EMR  |=  EXTI_Line11;       // 开事件（事件唤醒 STOP，不进 ISR）
	EXTI->RTSR &= ~EXTI_Line11;
	EXTI->FTSR |=  EXTI_Line11;
	EXTI->PR    =  EXTI_Line11;       // 清挂起

	/* ── EXTI12 事件模式：PA12(KEY2)下降沿唤醒 ── */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource12);
	EXTI->IMR  &= ~EXTI_Line12;
	EXTI->EMR  |=  EXTI_Line12;
	EXTI->RTSR &= ~EXTI_Line12;
	EXTI->FTSR |=  EXTI_Line12;
	EXTI->PR    =  EXTI_Line12;

	/* ── 进入 STOP 模式 ── */
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

	/* ========== 按键唤醒 ========== */
	SystemInit();

	/* 等待按键释放，避免 Key_GetNum 再次捕获本次唤醒的按键 */
	{
		uint32_t t = Delay_GetTick();
		while (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN1) == 0) {
			if (Delay_GetTick() - t > 500) break;
		}
		Delay_ms(20);
	}

	/* ── 恢复超声波 EXTI11 配置 ── */
	AFIO->EXTICR[2] = save_exticr2;
	EXTI->IMR = save_imr;
	EXTI->EMR = save_emr;
	EXTI->RTSR = save_rtsr;
	EXTI->FTSR = save_ftsr;
	EXTI->PR = EXTI_Line11 | EXTI_Line12;   // 清残留挂起

	current_state = STATE_MODE_SELECT;
}

static void ModeSelectMode(void)
{
	static uint8_t show_info = 0;

	if (!show_info) {
		OLED_Clear();
		OLED_ShowString(1, 1, "SELECT MODE:");
		OLED_ShowString(2, 1, "KEY1: Tracking");
		OLED_ShowString(3, 1, "KEY2: Obstacle");
		show_info = 1;
	}
}

static void StartupSelectMode(void)
{
	static uint8_t show_info = 0;
	if (!show_info) {
		OLED_Clear();
		OLED_ShowString(1, 1, "START TYPE:");
		OLED_ShowString(2, 1, "KEY1: Immediate");
		OLED_ShowString(3, 1, "KEY2: Timed");
		show_info = 1;
	}
}

static void TimedSetupMode(void)
{
	// main.c 负责按键交互，此处仅做展示占位
	OLED_ShowString(4, 1, "Set time with keys");
}

static void TrackingMode(void)
{
	static uint32_t last_time = 0;
	uint32_t now = Delay_GetTick();
	uint32_t distance;
	uint8_t pir_state;
	uint16_t pos;

	if (now - last_time >= 50) {
		last_time = now;

		distance = Ultrasonic_GetDistance();
		pir_state = PIR_examine();

		// 位置检测
		pos = Position_Get(POSITION_MARKERS);
		if (pos == 0) pos = POSITION_MARKERS;

		OLED_ShowString(1, 1, "TRACKING");
		OLED_ShowString(1, 10, "Dist:");
		OLED_ShowNum(1, 15, distance, 4);

		// 显示位置: "Pos: x/10"
		OLED_ShowString(2, 1, "Pos:");
		OLED_ShowNum(2, 5, pos, 2);
		OLED_ShowChar(2, 7, '/');
		OLED_ShowNum(2, 8, POSITION_MARKERS, 2);

		if (pir_state != 0) {
			TriggerHumanAlarm();
			current_state = STATE_HUMAN_ALARM;
			return;
		}

		if (distance < SAFE_DISTANCE && distance > 0) {
			TriggerObstacleAlarm();
			current_state = STATE_OBSTACLE_ALARM;
			return;
		}

		/* ── 循迹控制 ── */
		uint8_t sensor_code = Track_GetSensorCode();

		// 全高/全低 → 制动停车（脱离轨道或到达终点）
		if (sensor_code == 0x00 || sensor_code == 0x1F) {
			Car_Brake();
			OLED_ShowString(3, 1, "LOST/BRAKE");
			OLED_ShowString(4, 1, "L:  0 R:  0");
			return;
		}

		float error = Track_GetError();
		float pid_output = Track_PIDControl(error);

		int16_t left_speed = BASE_SPEED - pid_output;
		int16_t right_speed = BASE_SPEED + pid_output;

		if (left_speed > 99) left_speed = 99;
		if (left_speed < 0) left_speed = 0;
		if (right_speed > 99) right_speed = 99;
		if (right_speed < 0) right_speed = 0;

		Motor_L_Setspeed(left_speed);
		Motor_R_Setspeed(right_speed);

		OLED_ShowString(3, 1, "Err:");
		OLED_ShowNum(3, 5, (int16_t)(error * 10), 3);

		OLED_ShowString(4, 1, "L:");
		OLED_ShowNum(4, 3, left_speed, 2);
		OLED_ShowString(4, 6, "R:");
		OLED_ShowNum(4, 8, right_speed, 2);
	}
}

static void ObstacleAvoidanceMode(void)
{
	static uint32_t last_time = 0;
	static uint8_t obstacle_avoiding = 0;
	static uint32_t avoid_start_time = 0;
	uint32_t now = Delay_GetTick();
	uint32_t distance;
	uint8_t pir_state;
	uint16_t pos;

	if (now - last_time >= 50) {
		last_time = now;

		distance = Ultrasonic_GetDistance();
		pir_state = PIR_examine();

		pos = Position_Get(POSITION_MARKERS);
		if (pos == 0) pos = POSITION_MARKERS;

		OLED_ShowString(1, 1, "OBSTACLE");
		OLED_ShowString(1, 10, "Dist:");
		OLED_ShowNum(1, 15, distance, 4);

		OLED_ShowString(2, 1, "Pos:");
		OLED_ShowNum(2, 5, pos, 2);
		OLED_ShowChar(2, 7, '/');
		OLED_ShowNum(2, 8, POSITION_MARKERS, 2);

		if (pir_state != 0) {
			TriggerHumanAlarm();
			current_state = STATE_HUMAN_ALARM;
			return;
		}

		if (distance < SAFE_DISTANCE && distance > 0 && !obstacle_avoiding) {
			obstacle_avoiding = 1;
			avoid_start_time = now;

			Car_Brake();
			Delay_ms(500);

			LED_ON();
			Buzzer_ON();
			ESP_SendAlarm(0x02);

			OLED_ShowString(3, 1, "OBSTACLE! Avoiding...");
		}

		if (obstacle_avoiding) {
			if (now - avoid_start_time < 1500) {
				Car_GoBackward(40);
				OLED_ShowString(4, 1, "BACKING");
			} else if (now - avoid_start_time < 2500) {
				Car_SpinRight(50);
				OLED_ShowString(4, 1, "SPIN R");
			} else {
				obstacle_avoiding = 0;
				LED_OFF();
				Buzzer_OFF();
			}
		} else {
			Car_GoForward(BASE_SPEED);
			OLED_ShowString(4, 1, "L:60 R:60");
		}
	}
}

static void RemoteControlMode(void)
{
	// 遥控状态: 不控制电机（由 ESP 遥控指令驱动），只做传感器监测
	static uint32_t last_time = 0;
	uint32_t now = Delay_GetTick();
	uint16_t pos;

	if (now - last_time >= 200) {
		last_time = now;
		uint8_t pir_state = PIR_examine();

		pos = Position_Get(POSITION_MARKERS);
		if (pos == 0) pos = POSITION_MARKERS;

		OLED_ShowString(1, 1, "REMOTE CTRL");

		OLED_ShowString(2, 1, "Pos:");
		OLED_ShowNum(2, 5, pos, 2);
		OLED_ShowChar(2, 7, '/');
		OLED_ShowNum(2, 8, POSITION_MARKERS, 2);

		OLED_ShowString(3, 1, "Cmd:");
		OLED_ShowChar(3, 5, last_remote_cmd);

		OLED_ShowString(4, 1, pir_state ? "Person!" : "Safe   ");

		if (pir_state != 0) {
			TriggerHumanAlarm();
			current_state = STATE_HUMAN_ALARM;
		}
	}
}

static void ObstacleAlarmMode(void)
{
	static uint32_t last_time = 0;
	uint32_t now = Delay_GetTick();
	uint32_t distance;

	Car_Stop();

	if (now - last_time >= 100) {
		last_time = now;

		distance = Ultrasonic_GetDistance();
		OLED_ShowString(3, 1, "Dist:");
		OLED_ShowNum(3, 6, distance, 4);
		OLED_ShowString(3, 11, "mm");

		if (distance >= SAFE_DISTANCE || distance == 0) {
			OLED_Clear();
			OLED_ShowString(1, 1, "CLEARED");
			OLED_ShowString(2, 1, "Resuming...");
			Delay_ms(1000);
			ClearAlarms();
			current_state = (current_mode == MODE_TRACKING) ? STATE_TRACKING : STATE_OBSTACLE_AVOIDANCE;
		}
	}
}

static void HumanAlarmMode(void)
{
	static uint32_t last_time = 0;
	uint32_t now = Delay_GetTick();
	uint8_t pir_state;

	Car_Stop();

	if (now - last_time >= 100) {
		last_time = now;

		pir_state = PIR_examine();

		if (pir_state == 0 && human_detected) {
			OLED_Clear();
			OLED_ShowString(1, 1, "PERSON LEFT");
			OLED_ShowString(2, 1, "Resuming...");
			Delay_ms(1000);
			ClearAlarms();
			current_state = (current_mode == MODE_TRACKING) ? STATE_TRACKING : STATE_OBSTACLE_AVOIDANCE;
		}
	}
}

void StateMachine_Update(void)
{
	switch (current_state) {
		case STATE_STANDBY:
			StandbyMode();
			break;

		case STATE_MODE_SELECT:
			ModeSelectMode();
			break;

		case STATE_STARTUP_SELECT:
			StartupSelectMode();
			break;

		case STATE_TIMED_SETUP:
			TimedSetupMode();
			break;

		case STATE_TRACKING:
			TrackingMode();
			break;

		case STATE_OBSTACLE_AVOIDANCE:
			ObstacleAvoidanceMode();
			break;

		case STATE_REMOTE_CONTROL:
			RemoteControlMode();
			break;

		case STATE_OBSTACLE_ALARM:
			ObstacleAlarmMode();
			break;

		case STATE_HUMAN_ALARM:
			HumanAlarmMode();
			break;

		default:
			current_state = STATE_STANDBY;
			break;
	}
}
