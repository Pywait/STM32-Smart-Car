#ifndef __ESP_H
#define __ESP_H

#include "stm32f10x.h"

void ESP_Init(void);
void ESP_SendString(uint8_t* str);
uint8_t ESP_ReceiveByte(void);
void ESP_Reset(void);

#endif
