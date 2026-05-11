#ifndef __STATEMACHINE_H
#define __STATEMACHINE_H

#include "stm32f10x.h"

typedef enum {
    STATE_STANDBY = 0,
    STATE_MODE_SELECT,
    STATE_TRACKING,
    STATE_OBSTACLE_AVOIDANCE,
    STATE_OBSTACLE_ALARM,
    STATE_HUMAN_ALARM
} SystemState;

typedef enum {
    MODE_TRACKING = 1,
    MODE_OBSTACLE_AVOIDANCE = 2
} OperationMode;

#define SAFE_DISTANCE 200     // 安全距离（mm）
#define BASE_SPEED 60         // 基础速度
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

#endif
