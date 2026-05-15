#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include <stdio.h>

#define BLACK_LEVEL 1                    // 黑线检测电平（高电平表示检测到黑线）

// 权重数组：-2(最左) -1(左中) 0(中间) 1(右中) 2(最右)
static const int8_t weights[5] = {-2, -1, 0, 1, 2}; // 位置权重：左负右正

// PID控制参数
static float Kp = 12.0f;                 // 比例系数
static float Ki = 0.5f;                  // 积分系数
static float Kd = 2.0f;                  // 微分系数
static float integral = 0.0f;            // 积分累积
static float last_error = 0.0f;          // 上次误差
static float max_integral = 100.0f;      // 积分限幅

void Track_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;          // 下拉输入模式
	GPIO_InitStructure.GPIO_Pin = TRACK_L1_PIN | TRACK_L2_PIN | TRACK_R1_PIN | TRACK_R2_PIN | TRACK_M_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRACK_PORT, &GPIO_InitStructure);
}

uint8_t Track_GetSensorCode(void)
{
	uint8_t SensorCode = 0;
	
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_L1_PIN) == BLACK_LEVEL)
		SensorCode |= (1 << 4);
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_L2_PIN) == BLACK_LEVEL)
		SensorCode |= (1 << 3);
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_M_PIN) == BLACK_LEVEL)
		SensorCode |= (1 << 2);
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_R2_PIN) == BLACK_LEVEL)
		SensorCode |= (1 << 1);
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_R1_PIN) == BLACK_LEVEL)
		SensorCode |= (1 << 0);

	return SensorCode;
}
//if……else if……else，只能接收一个传感器的数据；switch……case是针对同一个表达式的不同值进行跳转

// 获取循迹偏差值（-2到+2，表示偏离中心线的程度）
float Track_GetError(void)
{
    uint8_t sensor_code = Track_GetSensorCode();
    int16_t sum_weight = 0;
    uint8_t active_count = 0;
    
    // 计算加权误差
    for (uint8_t i = 0; i < 5; i++)
    {
        if (sensor_code & (1 << (4 - i)))  // L1对应bit4, R1对应bit0
        {
            sum_weight += weights[i];
            active_count++;
        }
    }
    
    // 如果没有传感器检测到黑线，保持上次误差方向
    if (active_count == 0)
    {
        if (last_error > 0) return 2;   // 继续向右找线
        else if (last_error < 0) return -2; // 继续向左找线
        else return 0;                   // 初始状态
    }
    float error = (float)sum_weight / active_count;
    return error;
}

// PID控制计算，返回转向修正量（负值左转，正值右转）
float Track_PIDControl(float error)
{
    float p_term = Kp * error;
    
    // 积分项（带限幅）
    integral += error;
    if (integral > max_integral) integral = max_integral;
    if (integral < -max_integral) integral = -max_integral;
    float i_term = Ki * integral;
    
    // 微分项
    float d_term = Kd * (error - last_error);
    last_error = error;
    
    // 总控制输出
    return p_term + i_term + d_term;
}

// 获取传感器状态字符串（用于调试）
void Track_GetStatusString(char* str)
{
    uint8_t code = Track_GetSensorCode();
    sprintf(str, "L1:%d L2:%d M:%d R2:%d R1:%d",
            (code & 0x10) ? 1 : 0,
            (code & 0x08) ? 1 : 0,
            (code & 0x04) ? 1 : 0,
            (code & 0x02) ? 1 : 0,
            (code & 0x01) ? 1 : 0);
}

// 简化版循迹决策（无PID，直接返回转向指令）
// 返回值：-1=左转, 0=直行, 1=右转
int8_t Track_GetDirection(void)
{
    uint8_t sensor_code = Track_GetSensorCode();
    
    // 判断各种情况
    if (sensor_code == 0x04)       // 只有中间检测到
        return 0;                   // 直行
        
    if (sensor_code == 0x08 ||      // 只有L2检测到
        sensor_code == 0x18)        // L1+L2检测到
        return -1;                  // 左转
        
    if (sensor_code == 0x02 ||      // 只有R2检测到
        sensor_code == 0x03)        // R1+R2检测到
        return 1;                   // 右转
        
    if (sensor_code == 0x10)        // 只有L1检测到（严重偏右）
        return -1;                  // 强力左转
        
    if (sensor_code == 0x01)        // 只有R1检测到（严重偏左）
        return 1;                   // 强力右转
        
    if (sensor_code == 0x0C)        // L2+M检测到
        return -1;                  // 轻微左转
        
    if (sensor_code == 0x06)        // M+R2检测到
        return 1;                   // 轻微右转
        
    // 默认直行
    return 0;
}
