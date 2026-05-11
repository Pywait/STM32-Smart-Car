#include "StateMachine.h"
#include "Delay.h"
#include "LED.h"
#include "Buzzer.h"
#include "ISD.h"
#include "ESP.h"
#include "Car.h"
#include "Ultrasonic.h"
#include "PIR.h"
#include "Track.h"
#include "OLED.h"

SystemState current_state = STATE_STANDBY;
OperationMode current_mode = MODE_TRACKING;
uint8_t alarm_active = 0;

static uint32_t alarm_start_time = 0;
static uint8_t human_detected = 0;

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
    alarm_start_time = Delay_GetTick();
    
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
    alarm_start_time = Delay_GetTick();
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
}

static void StandbyMode(void)
{
    OLED_ShowString(1, 1, "STANDBY");
    OLED_ShowString(2, 1, "Press KEY1 to start");
    
    Car_Stop();
    LED_OFF();
    Buzzer_OFF();
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

static void TrackingMode(void)
{
    static uint32_t last_time = 0;
    uint32_t now = Delay_GetTick();
    uint32_t distance;
    uint8_t pir_state;
    
    if (now - last_time >= 50) {
        last_time = now;
        
        distance = Ultrasonic_GetDistance();
        pir_state = PIR_examine();
        
        OLED_Clear();
        OLED_ShowString(1, 1, "TRACKING MODE");
        OLED_ShowString(2, 1, "Dist:");
        OLED_ShowNum(2, 6, distance, 4);
        OLED_ShowString(2, 11, "mm");
        
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
        
        OLED_ShowString(3, 1, "Error:");
        OLED_ShowNum(3, 7, (int16_t)(error * 10), 3);
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
    
    if (now - last_time >= 50) {
        last_time = now;
        
        distance = Ultrasonic_GetDistance();
        pir_state = PIR_examine();
        
        OLED_Clear();
        OLED_ShowString(1, 1, "OBSTACLE MODE");
        OLED_ShowString(2, 1, "Dist:");
        OLED_ShowNum(2, 6, distance, 4);
        OLED_ShowString(2, 11, "mm");
        
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
            } else if (now - avoid_start_time < 2500) {
                Car_SpinRight(50);
            } else {
                obstacle_avoiding = 0;
                LED_OFF();
                Buzzer_OFF();
            }
        } else {
            Car_GoForward(BASE_SPEED);
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
            
        case STATE_TRACKING:
            TrackingMode();
            break;
            
        case STATE_OBSTACLE_AVOIDANCE:
            ObstacleAvoidanceMode();
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
