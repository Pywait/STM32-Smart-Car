#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "Delay.h"

#define PIR_LOCKOUT_MS 2500   // 2.5s 封锁时间

void PIR_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = PIR_FRONT_PIN | PIR_BACK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PIR_PORT, &GPIO_InitStructure);
}

/**
  * @brief  被动红外检测（含 2.5s 封锁期）。
  * @retval 0=无人, 1=前方有人, 2=后方有人。
  *         每次检测到触发后，2.5s 内不再响应，避免重复误报。
  */
uint8_t PIR_examine(void)
{
	static uint32_t last_trig_time = 0;
	uint32_t now = Delay_GetTick();

	/* 2.5s 封锁期内不响应 */
	if (now - last_trig_time < PIR_LOCKOUT_MS)
		return 0;

	uint8_t PIR_examine_Num = 0;

	if (GPIO_ReadInputDataBit(PIR_PORT, PIR_FRONT_PIN) == 1)
	{
		Delay_us(10);
		if (GPIO_ReadInputDataBit(PIR_PORT, PIR_FRONT_PIN) == 1) {
			PIR_examine_Num = 1;
			last_trig_time = now;
		}
	}
	if (GPIO_ReadInputDataBit(PIR_PORT, PIR_BACK_PIN) == 1)
	{
		Delay_us(10);
		if (GPIO_ReadInputDataBit(PIR_PORT, PIR_BACK_PIN) == 1) {
			PIR_examine_Num = 2;
			last_trig_time = now;
		}
	}

	return PIR_examine_Num;
}
