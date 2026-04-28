#ifndef __USART2_H
#define __USART2_H

void USART2_Init(void);
void USART2_SendByte(uint8_t data);
void USART2_SendString(char *str);

uint8_t USART2_ReceiveByte(void);

#endif
