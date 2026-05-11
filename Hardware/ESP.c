#include "stm32f10x.h"
#include "hardware.h"
#include "Delay.h"
#include "Motor.h"      // 需要使用电机控制函数
#include <string.h>

// 环形缓冲区
#define RX_BUF_SIZE 256
static volatile uint8_t rx_buffer[RX_BUF_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

// 向环形缓冲区写入一个字节（中断中调用）
static void rx_buffer_put(uint8_t data)
{
    uint16_t next = (rx_head + 1) % RX_BUF_SIZE;
    if (next != rx_tail) {
        rx_buffer[rx_head] = data;
        rx_head = next;
    }
    // 如果满则丢弃（可根据需要添加溢出计数）
}

// 从环形缓冲区读取一个字节（非阻塞，返回0表示无数据）
static uint8_t rx_buffer_get(uint8_t *data)
{
    if (rx_head == rx_tail) return 0;
    *data = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
    return 1;
}

// USART1 中断服务函数（接收数据）
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART1);
        rx_buffer_put(data);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

void ESP_Init(void)
{
    // 时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    // 引脚配置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = ESP_USART1_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ESP_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = ESP_USART1_RX_PIN;
    GPIO_Init(ESP_PORT, &GPIO_InitStructure);
    
    // USART1 配置
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);
    
    // 初始化环形缓冲区指针
    rx_head = 0;
    rx_tail = 0;
	
    // 使能接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    // NVIC 配置
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(USART1, ENABLE);
    
}

// 发送单字节（等待TXE标志）
void ESP_SendByte(uint8_t data)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, data);
}

// 发送字符串
void ESP_SendString(uint8_t* str)
{
    while (*str)
    {
        ESP_SendByte(*str++);
    }
}

// 发送多字节数组
void ESP_SendBytes(uint8_t* data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        ESP_SendByte(data[i]);
    }
}

// 阻塞读取一个字节（从环形缓冲区）
uint8_t ESP_ReceiveByte(void)
{
    uint8_t data;
    while (!rx_buffer_get(&data));
    return data;
}

// 上报报警：type 0x01人员闯入, 0x02障碍物
void ESP_SendAlarm(uint8_t type)
{
    uint8_t buf[2] = {0x10, type};
    ESP_SendBytes(buf, 2);
}

// 上报位置：idx当前点(1~total), total总点数
void ESP_SendPosition(uint8_t idx, uint8_t total)
{
    uint8_t buf[3] = {0x20, idx, total};
    ESP_SendBytes(buf, 3);
}

// 解析APP下行指令，需在主循环中频繁调用
void ESP_ProcessCommands(void)
{
    uint8_t data;
    while (rx_buffer_get(&data))
    {
        switch (data)
        {
            case 0x01:  // 前进
                Motor_L_Setspeed(80);
                Motor_R_Setspeed(80);
                break;
            case 0x02:  // 后退
                Motor_L_Setspeed(-80);
                Motor_R_Setspeed(-80);
                break;
            case 0x03:  // 左自旋
                Motor_L_Setspeed(-80);
                Motor_R_Setspeed(80);
                break;
            case 0x04:  // 右自旋
                Motor_L_Setspeed(80);
                Motor_R_Setspeed(-80);
                break;
            default:
                // 未知指令，丢弃
                break;
        }
    }
}