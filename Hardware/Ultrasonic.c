#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "Delay.h"

uint8_t Ultrasonic_Flag = 0;
uint16_t Ultrasonic_Timer = 0;
uint32_t Ultrasonic_Distance = 0;

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
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;					//分频72
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);							//定时器中断配置
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	
	EXTI_InitTypeDef EXTI_InitStructure;								//Extern Interrupt外部中断
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;						//嵌套向量中断控制器中断请求通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;			//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					//响应优先级
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, DISABLE);	
}

uint32_t Ultrasonic_GetDistance(void)
{
	uint16_t TimeOut = 0;

	Delay_ms(200);			//修改！
	GPIO_SetBits(ULTRASONIC_PORT, ULTRASONIC_TX_PIN);
	Delay_us(15);
	GPIO_ResetBits(ULTRASONIC_PORT, ULTRASONIC_TX_PIN);

	while (Ultrasonic_Flag == 0)
	{
		TimeOut++;
		if (TimeOut > 10000)
		{
			return 0;
		}
	}
	Ultrasonic_Flag = 0;

	return Ultrasonic_Distance;
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
		if (Ultrasonic_Flag == 0)
		{
			Ultrasonic_Flag = 1;
			TIM_SetCounter(TIM2, 0);
			TIM_Cmd(TIM2, ENABLE);
		}
		else if (Ultrasonic_Flag == 1)
		{
			Ultrasonic_Flag = 0;
			TIM_Cmd(TIM2, DISABLE);
			Ultrasonic_Distance = (Ultrasonic_Timer * 1000 + TIM_GetCounter(TIM2)) * 342.62 / 2000;	//us -> s ; m -> mm
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
