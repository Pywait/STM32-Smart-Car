#ifndef __USART2_H
#define __USART2_H

void USART2_Init(void);
void USART2_SendByte(uint8_t data);
void USART2_SendArray(uint8_t *Array, uint16_t Length);
void USART2_SendString(char *str);
void USART2_SendNumber(uint32_t Number, uint8_t Length);

uint8_t USART2_GetRxFlag(void);
uint8_t USART2_GetRxData(void);

#endif
