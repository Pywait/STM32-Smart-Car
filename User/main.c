/*
 * 分轮验证主程序
 *
 * 使用方法：修改 ROUND 宏的值，保留当前要验证的轮次，其它轮次的代码
 * 在编译时自动排除，无需手动注释。
 *
 *   ROUND 1 — OLED + 按键 + LED + 蜂鸣器     (基础外设)
 *   ROUND 2 — PWM + 电机 + 小车动作           (执行器)
 *   ROUND 3 — 超声波 + 循迹 + PIR + 位置检测  (传感器)
 *   ROUND 4 — ESP-01S + 巴法云通信            (通信)
 *   ROUND 5 — 子系统组合 (循迹/避障/报警)     (功能整合)
 *   ROUND 6 — 完整状态机                      (全系统)
 */
#define ROUND  1

#include "stm32f10x.h"
#include "hardware.h"
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "Buzzer.h"
#include "ISD.h"
#include "Ultrasonic.h"
#include "PIR.h"
#include "PWM.h"
#include "Motor.h"
#include "Car.h"
#include "Track.h"
#include "Position.h"
#include "ESP.h"
#include "StateMachine.h"

/* ========== ROUND 1: OLED + 按键 + LED + 蜂鸣器 ========== */
#if ROUND == 1

int main(void)
{
    OLED_Init();
    Key_Init();
    LED_Init();
    Buzzer_Init();

    OLED_ShowString(1, 1, "=== ROUND 1 ===");
    OLED_ShowString(2, 1, "KEY1: LED");
    OLED_ShowString(3, 1, "KEY2: Buzzer");
    OLED_ShowString(4, 1, "Ready!");

    uint8_t key_num;
    uint8_t led_state = 0;
    uint8_t buz_state = 0;

    while (1)
    {
        key_num = Key_GetNum();

        if (key_num == 1) {
            led_state = !led_state;
            if (led_state) { LED_ON();  OLED_ShowString(4, 1, "LED ON "); }
            else           { LED_OFF(); OLED_ShowString(4, 1, "LED OFF"); }
        }

        if (key_num == 2) {
            buz_state = !buz_state;
            if (buz_state) { Buzzer_ON();  OLED_ShowString(4, 9, "BUZ ON "); }
            else           { Buzzer_OFF(); OLED_ShowString(4, 9, "BUZ OFF"); }
        }

        Delay_ms(20);
    }
}

/* ========== ROUND 2: PWM + 电机 + 小车动作 ========== */
#elif ROUND == 2

int main(void)
{
    OLED_Init();
    Key_Init();
    PWM_Init();
    Motor_Init();
    Car_Init();

    OLED_ShowString(1, 1, "=== ROUND 2 ===");
    OLED_ShowString(2, 1, "MOTOR TEST");
    OLED_ShowString(3, 1, "Auto sequence");
    OLED_ShowString(4, 1, "5s each step");

    /* 自动动作序列 */
    const char* actions[] = {
        "FWD 60",
        "STOP",
        "BWD 50",
        "LEFT",
        "RIGHT",
        "SPIN L",
        "SPIN R",
        "BRAKE",
        "STOP"
    };
    uint8_t step = 0;
    uint32_t step_start = Delay_GetTick();

    while (1)
    {
        uint32_t now = Delay_GetTick();

        /* 每 5 秒切换到下一步 */
        if (now - step_start >= 5000) {
            step_start = now;
            step = (step + 1) % 9;
        }

        switch (step) {
        case 0: Car_GoForward(60);   break;
        case 1: Car_Stop();          break;
        case 2: Car_GoBackward(50);  break;
        case 3: Car_TurnLeft(60);    break;
        case 4: Car_TurnRight(60);   break;
        case 5: Car_SpinLeft(60);    break;
        case 6: Car_SpinRight(60);   break;
        case 7: Car_Brake();         break;
        case 8: Car_Stop();          break;
        }

        OLED_ShowString(4, 1, actions[step]);
        OLED_ShowString(4, 10, "    ");

        /* 按 KEY1 强制下一步 */
        if (Key_GetNum() == 1) {
            step_start = now;
            step = (step + 1) % 9;
        }

        Delay_ms(50);
    }
}

/* ========== ROUND 3: 传感器（超声波 + 循迹 + PIR + 位置）= */
#elif ROUND == 3

int main(void)
{
    OLED_Init();
    Key_Init();
    Ultrasonic_Init();
    Track_Init();
    PIR_Init();
    Position_Init();

    OLED_ShowString(1, 1, "=== ROUND 3 ===");
    OLED_ShowString(2, 1, "SENSOR TEST");

    uint8_t pir_prev = 0;

    while (1)
    {
        /* ── 超声波 ── */
        Ultrasonic_Update();
        uint32_t dist = Ultrasonic_GetDistance();
        OLED_ShowString(2, 1, "Dist:");
        OLED_ShowNum(2, 6, dist, 4);
        OLED_ShowString(2, 11, "mm  ");

        /* ── 循迹传感器 ── */
        uint8_t track_code = Track_GetSensorCode();
        OLED_ShowString(3, 1, "Track:");
        /* 显示 5 位二进制 */
        OLED_ShowChar(3, 7, (track_code & 0x10) ? '1' : '0');
        OLED_ShowChar(3, 8, (track_code & 0x08) ? '1' : '0');
        OLED_ShowChar(3, 9, (track_code & 0x04) ? '1' : '0');
        OLED_ShowChar(3, 10, (track_code & 0x02) ? '1' : '0');
        OLED_ShowChar(3, 11, (track_code & 0x01) ? '1' : '0');
        OLED_ShowString(3, 13, "   ");

        float err = Track_GetError();
        OLED_ShowString(3, 13, "E:");
        OLED_ShowNum(3, 15, (int16_t)(err * 10), 3);

        /* ── PIR ── */
        uint8_t pir = PIR_examine();
        if (pir != pir_prev) {
            pir_prev = pir;
            if (pir)
                OLED_ShowString(4, 1, "PIR: PERSON! ");
            else
                OLED_ShowString(4, 1, "PIR: safe    ");
        }

        /* ── 位置（霍尔） ── */
        uint16_t pos = Position_Get(10);
        if (pos == 0) pos = 10;
        OLED_ShowString(4, 13, "P:");
        OLED_ShowNum(4, 15, pos, 2);

        /* KEY1 重置位置计数 */
        if (Key_GetNum() == 1) {
            /* 通过重新初始化来重置位置计数 */
            Position_Init();
            OLED_Clear();
            OLED_ShowString(1, 1, "=== ROUND 3 ===");
            OLED_ShowString(2, 1, "Pos reset!");
            Delay_ms(500);
        }

        Delay_ms(100);
    }
}

/* ========== ROUND 4: ESP-01S + 巴法云通信 ========== */
#elif ROUND == 4

int main(void)
{
    OLED_Init();
    Key_Init();
    PWM_Init();
    Motor_Init();
    Car_Init();
    ESP_Init();

    OLED_ShowString(1, 1, "=== ROUND 4 ===");
    OLED_ShowString(2, 1, "ESP connecting.");
    Delay_ms(500);
    OLED_ShowString(2, 1, "ESP connecting..");
    Delay_ms(500);
    OLED_ShowString(2, 1, "ESP connecting...");

    /* 连接巴法云 */
    ESP_MQTTInit();

    OLED_Clear();
    OLED_ShowString(1, 1, "=== ROUND 4 ===");
    OLED_ShowString(2, 1, "Bemfa connected!");
    OLED_ShowString(3, 1, "Send cmd from APP");
    OLED_ShowString(4, 1, "Cmd: ");

    /* 测试上报一条位置数据 */
    Delay_ms(2000);
    ESP_SendPosition(5);
    OLED_ShowString(4, 10, "Pos sent");

    uint8_t test_alarm_sent = 0;

    while (1)
    {
        ESP_ProcessCommands();

        /* 每 10 秒发一次位置 + 电池，验证上行 */
        static uint32_t last_report = 0;
        uint32_t now = Delay_GetTick();
        if (now - last_report > 10000) {
            last_report = now;

            if (!test_alarm_sent) {
                ESP_SendAlarm(0x01);  /* 发一条 person 测试 */
                test_alarm_sent = 1;
                OLED_ShowString(4, 1, "Alarm sent!");
            } else {
                ESP_SendPosition(5);
                ESP_SendBattery(85);
                OLED_ShowString(4, 1, "Pos+Bat sent");
            }
        }

        Delay_ms(50);
    }
}

/* ========== ROUND 5: 子系统组合（循迹 / 避障 / 报警）= */
#elif ROUND == 5

/*
 * 手动切换模式：
 *   KEY1 → 循迹模式 (TRACKING)
 *   KEY2 → 避障模式 (OBSTACLE AVOIDANCE)
 *   检测到人 → 人体报警（声光 + 云上报，按 KEY2 清除）
 *   超声波 < 安全距离 → 障碍报警（避障模式下自动躲闪）
 */

int main(void)
{
    OLED_Init();
    LED_Init();
    Key_Init();
    Buzzer_Init();
    ISD_Init();
    Ultrasonic_Init();
    PIR_Init();
    Track_Init();
    Position_Init();
    PWM_Init();
    Motor_Init();
    Car_Init();
    ESP_Init();

    StateMachine_Init();

    /* 连接巴法云 */
    OLED_ShowString(1, 1, "Bemfa connecting...");
    ESP_MQTTInit();
    OLED_Clear();
    OLED_ShowString(1, 1, "Bemfa OK");
    Delay_ms(500);

    /* 默认进入循迹模式 */
    current_state = STATE_TRACKING;
    current_mode = MODE_TRACKING;
    OLED_ShowString(2, 1, "TRACKING MODE");
    OLED_ShowString(4, 1, "KEY1=Trk KEY2=Obs");

    uint8_t key_num;

    while (1)
    {
        key_num = Key_GetNum();

        if (key_num == 1) {
            OLED_Clear();
            current_mode = MODE_TRACKING;
            current_state = STATE_TRACKING;
            OLED_ShowString(1, 1, "TRACKING MODE");
            OLED_ShowString(4, 1, "KEY1=Trk KEY2=Obs");
        }

        if (key_num == 2) {
            /* 如果当前是报警状态，KEY2 清除报警 */
            if (current_state == STATE_OBSTACLE_ALARM ||
                current_state == STATE_HUMAN_ALARM) {
                ClearAlarms();
                OLED_Clear();
                OLED_ShowString(1, 1, "ALARM CLEARED");
                Delay_ms(500);
                current_state = (current_mode == MODE_TRACKING)
                                ? STATE_TRACKING : STATE_OBSTACLE_AVOIDANCE;
                OLED_ShowString(4, 1, "KEY1=Trk KEY2=Obs");
            } else {
                OLED_Clear();
                current_mode = MODE_OBSTACLE_AVOIDANCE;
                current_state = STATE_OBSTACLE_AVOIDANCE;
                OLED_ShowString(1, 1, "OBSTACLE MODE");
                OLED_ShowString(4, 1, "KEY1=Trk KEY2=Obs");
            }
        }

        /* 运行状态机（包含传感器采样、电机控制、报警逻辑） */
        StateMachine_Update();
        Ultrasonic_Update();
        ESP_ProcessCommands();

        Delay_ms(10);
    }
}

/* ========== ROUND 6: 完整状态机（全系统） ========== */
#elif ROUND == 6

/* 定时启动参数 */
static uint16_t timed_hours = 0;
static uint16_t timed_minutes = 5;
static uint8_t  timed_setting_hours = 1;
static uint8_t  timed_setup_entered = 0;
static uint32_t timed_last_activity = 0;

static void RTC_EnterTimedStartup(uint16_t minutes)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    RCC_LSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();

    RTC_SetPrescaler(40000 - 1);
    RTC_WaitForLastTask();

    RTC_SetCounter(0);
    RTC_WaitForLastTask();
    RTC_SetAlarm(minutes * 60);
    RTC_WaitForLastTask();

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    RTC_WaitForLastTask();

    OLED_Clear();
    OLED_ShowString(1, 1, "TIMED START");
    OLED_ShowString(2, 1, "Sleeping...");
    Delay_ms(300);

    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    SystemInit();

    if (RTC_GetFlagStatus(RTC_FLAG_ALR) != RESET) {
        RTC_ClearFlag(RTC_FLAG_ALR);
        RTC_WaitForLastTask();
    }

    OLED_Clear();
    OLED_ShowString(1, 1, "TIME'S UP!");
    Delay_ms(300);
}

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

    OLED_ShowString(1, 1, "Bemfa connecting...");
    ESP_MQTTInit();
    OLED_Clear();
    OLED_ShowString(1, 1, "Bemfa OK");

    uint8_t KeyNum;

    while (1)
    {
        KeyNum = Key_GetNum();

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
                OLED_Clear();
                if (current_mode == MODE_TRACKING) {
                    current_state = STATE_TRACKING;
                    OLED_ShowString(1, 1, "TRACKING MODE");
                } else {
                    current_state = STATE_OBSTACLE_AVOIDANCE;
                    OLED_ShowString(1, 1, "OBSTACLE MODE");
                }
            } else if (KeyNum == 2) {
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

            if (Delay_GetTick() - timed_last_activity > 3000) {
                uint16_t total_min = timed_hours * 60 + timed_minutes;
                if (total_min < 1) total_min = 1;
                if (total_min > 1440) total_min = 1440;

                if (current_mode == MODE_TRACKING)
                    current_state = STATE_TRACKING;
                else
                    current_state = STATE_OBSTACLE_AVOIDANCE;

                RTC_EnterTimedStartup(total_min);

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

#else
#error "ROUND must be 1-6"
#endif
