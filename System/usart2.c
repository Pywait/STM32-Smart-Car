#include "stm32f10x.h"                  // Device header
#include "hardware.h"

uint8_t usart2_RXData;
uint8_t usart2_RxFlag;

void USART2_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = usart2_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(usart2_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = usart2_RX_PIN;
	GPIO_Init(usart2_GPIO_PORT, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;		//baudrate;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;									//奇偶校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(usart2_USART_PORT, &USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
}

void USART2_SendByte(uint8_t data)
{
	USART_SendData(USART2, data);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
//	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

void USART2_SendArray(uint8_t *Array, uint16_t Length)		//why16位
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		USART2_SendByte(Array[i]);
	}
}

void USART2_SendString(char *str)
{
	while (*str)
	{
		USART2_SendByte((uint8_t)(*str++));
	}
//	uint16_t i = 0;
//	while (str[i] != '\0') 
//	{
//		USART2_SendByte((uint8_t)str[i]);
//		i++;
//	}
//*	------*/
//	uint8_t i;
//	for (i = 0; str[i] != '\0'; i ++)
//	{
//		USART2_SendByte(str[i]);
//	}
}

uint32_t Power(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void USART2_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		USART2_SendByte(Number / Power(10, Length - i - 1) % 10 + '0');
	}
}

//* 阻塞等待*/
//uint8_t USART2_ReceiveByte(void)
//{
//	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
//	return USART_ReceiveData(USART2);
//}

uint8_t USART2_GetRxFlag(void)
{
	if (usart2_RxFlag == 1)
	{
		usart2_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t USART2_GetRxData(void)
{
	return usart2_RXData;
}

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		usart2_RXData = USART_ReceiveData(USART2);
		usart2_RxFlag = 1;
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
