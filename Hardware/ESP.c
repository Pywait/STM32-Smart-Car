#include "stm32f10x.h"                  // Device header
#include "hardware.h"
#include "Delay.h"

void ESP_Init(void)
{
    // 初始化USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置USART1引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = ESP_USART1_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ESP_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = ESP_USART1_RX_PIN;
    GPIO_Init(ESP_PORT, &GPIO_InitStructure);
    
    // 配置USART1
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);
    
    // 使能USART1
    USART_Cmd(USART1, ENABLE);
    
    // 初始化GPIO0、GPIO2和RST引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Pin = ESP_GPIO0_PIN | ESP_GPIO2_PIN | ESP_RST_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ESP_PORT, &GPIO_InitStructure);
    
    // 复位ESP-01S
//    ESP_Reset();
}

void ESP_SendString(uint8_t* str)
{
    while (*str != '\0')
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1, *str);
        str++;
    }
}

uint8_t ESP_ReceiveByte(void)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USART1);
}

//void ESP_Reset(void)
//{
//    // 拉低RST引脚复位
//    GPIO_ResetBits(ESP_PORT, ESP_RST_PIN);
//    // 使用Delay函数延时
//    Delay_ms(100);  // 100ms延时
//    // 拉高RST引脚
//    GPIO_SetBits(ESP_PORT, ESP_RST_PIN);
//    // 延时等待ESP-01S启动
//    Delay_ms(1000);  // 1秒延时
//}
