#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "Delay.h"

volatile uint8_t Ultrasonic_Flag = 0;      // 0=idle/done, 1=等待下降沿
volatile uint16_t Ultrasonic_Timer = 0;
volatile uint32_t Ultrasonic_Distance = 0; // mm, 上次完整测距结果

static uint32_t last_trig_time = 0;
static uint8_t trig_pending = 0;           // 0=空闲, 1=已触发等待回响

void Ultrasonic_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = ULTRASONIC_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ULTRASONIC_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = ULTRASONIC_RX_PIN;
	GPIO_Init(ULTRASONIC_PORT, &GPIO_InitStructure);

	TIM_InternalClockConfig(TIM2);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM2, DISABLE);
}

/**
  * @brief  非阻塞测距状态机，主循环频繁调用。
  *         200ms 间隔触发一次超声，EXTI 中断捕获上升/下降沿并计算距离。
  *         30ms 无回响判定超时（覆盖约 5m 量程）。
  */
void Ultrasonic_Update(void)
{
	uint32_t now = Delay_GetTick();

	/* 等待当前测量完成或超时 */
	if (trig_pending)
	{
		if (Ultrasonic_Flag == 0) {
			/* 下降沿已到，测距完成 */
			trig_pending = 0;
		}
		else if (now - last_trig_time > 30) {
			/* 30ms 超时 —— 无回响 */
			TIM_Cmd(TIM2, DISABLE);
			Ultrasonic_Flag = 0;
			Ultrasonic_Distance = 0;
			trig_pending = 0;
		}
	}

	/* 空闲且满足 200ms 间隔 → 触发新测量 */
	if (!trig_pending && (now - last_trig_time >= 200))
	{
		GPIO_SetBits(ULTRASONIC_PORT, ULTRASONIC_TX_PIN);
		Delay_us(15);
		GPIO_ResetBits(ULTRASONIC_PORT, ULTRASONIC_TX_PIN);

		last_trig_time = now;
		trig_pending = 1;
	}
}

/* 返回缓存距离，不阻塞 */
uint32_t Ultrasonic_GetDistance(void)
{
	return Ultrasonic_Distance;
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
		if (Ultrasonic_Flag == 0)
		{
			Ultrasonic_Flag = 1;
			Ultrasonic_Timer = 0;
			TIM_SetCounter(TIM2, 0);
			TIM_Cmd(TIM2, ENABLE);
		}
		else if (Ultrasonic_Flag == 1)
		{
			TIM_Cmd(TIM2, DISABLE);
			Ultrasonic_Distance = (Ultrasonic_Timer * 1000 + TIM_GetCounter(TIM2)) * 343 / 2000;
			Ultrasonic_Flag = 0;
		}
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		Ultrasonic_Timer ++;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
