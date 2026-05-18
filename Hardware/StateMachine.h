#ifndef __STATEMACHINE_H
#define __STATEMACHINE_H

#include "stm32f10x.h"

typedef enum {
	STATE_STANDBY = 0,
	STATE_MODE_SELECT,
	STATE_STARTUP_SELECT,       // 选择立即启动 / 定时启动
	STATE_TIMED_SETUP,          // 设置定时时间
	STATE_TRACKING,
	STATE_OBSTACLE_AVOIDANCE,
	STATE_REMOTE_CONTROL,       // APP 遥控
	STATE_OBSTACLE_ALARM,
	STATE_HUMAN_ALARM
} SystemState;

typedef enum {
	MODE_TRACKING = 1,
	MODE_OBSTACLE_AVOIDANCE = 2,
	MODE_REMOTE = 3
} OperationMode;

#define SAFE_DISTANCE 200     // 安全距离（mm）
#define BASE_SPEED 85         // 基础速度
#define ALARM_DURATION 2000   // 报警持续时间（ms）

extern SystemState current_state;
extern OperationMode current_mode;
extern uint8_t alarm_active;

void StateMachine_Init(void);
void StateMachine_Update(void);
void StateMachine_SetState(SystemState state);
void StateMachine_SetMode(OperationMode mode);
void TriggerObstacleAlarm(void);
void TriggerHumanAlarm(void);
void ClearAlarms(void);
void RemoteCtrl_SetCmd(char cmd);

#endif
