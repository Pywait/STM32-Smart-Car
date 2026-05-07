#ifndef __TRACK_H
#define __TRACK_H

#include "stm32f10x.h"

void Track_Init(void);
uint8_t Track_GetSensorCode(void);
float Track_GetError(void);
float Track_PIDControl(int8_t error);
void Track_GetStatusString(char* str);
int8_t Track_GetDirection(void);

#endif
