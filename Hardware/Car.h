#ifndef __CAR_H
#define __CAR_H

void Car_Init(void);
void Car_GoForward(uint8_t speed);
void Car_GoBackward(uint8_t speed);
void Car_SpinLeft(uint8_t speed);
void Car_SpinRight(uint8_t speed);
void Car_TurnLeft(uint8_t speed);
void Car_TurnRight(uint8_t speed);
void Car_PID_TurnLeft(uint8_t speed_diff, uint8_t base_speed);   // 左转幅度可调
void Car_PID_TurnRight(uint8_t speed_diff, uint8_t base_speed);
void Car_Brake(void);
void Car_Stop(void);

#endif
