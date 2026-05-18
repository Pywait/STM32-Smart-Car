#ifndef __CAR_H
#define __CAR_H

void Car_Init(void);
void Car_GoForward(uint8_t speed);
void Car_GoBackward(uint8_t speed);
// void Car_SpinLeft(uint8_t speed);   // 非麦克纳姆轮，暂不使用
// void Car_SpinRight(uint8_t speed);
void Car_TurnLeft(uint8_t speed);
void Car_TurnRight(uint8_t speed);
void Car_Brake(void);
void Car_Stop(void);

#endif
