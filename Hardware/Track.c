#include "stm32f10x.h"                  // Device header
#include "hardware.h"

#define BLACK_LEVEL 1

static const uint16_t track_pins[5] = {
    TRACK_L1_PIN,
    TRACK_L2_PIN,
    TRACK_M_PIN,
    TRACK_R2_PIN,
    TRACK_R1_PIN
};

static const int8_t weights[5] = {-2, -1, 0, 1, 2};

// PID 参数（先用比例控制）
//static float Kp = 15.0f;
//static float Ki = 0.0f;
//static float Kd = 0.0f;
//static float integral = 0.0f;
//static float last_error = 0.0f;

void Track_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = TRACK_L1_PIN | TRACK_L2_PIN | TRACK_R1_PIN | TRACK_R2_PIN | TRACK_M_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRACK_PORT, &GPIO_InitStructure);
}

uint8_t Track_GetSensorCode(void)
{
	uint8_t SensorCode = 0;
	
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_L1_PIN) == 0)
		SensorCode |= (1 << 4);
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_L2_PIN) == 0)
		SensorCode |= (1 << 3);
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_M_PIN) == 0)
		SensorCode |= (1 << 2);
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_R2_PIN) == 0)
		SensorCode |= (1 << 1);
	if (GPIO_ReadInputDataBit(TRACK_PORT, TRACK_R1_PIN) == 0)
		SensorCode |= (1 << 0);

	return SensorCode;
}
//if……else if……else，只能接收一个传感器的数据；switch……case是针对同一个表达式的不同值进行跳转

//void Track_GetTrackWeight(void)
//{
//	
//}
