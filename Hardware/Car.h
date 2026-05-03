#ifndef __CAR_H
#define __CAR_H

void Motor_SelfLight(int8_t Lspeed, int8_t Rspeed);
void Car_Forward(uint8_t speed);
void Car_Backward(uint8_t speed);
void Car_TurnLeft(uint8_t speed_diff, uint8_t base_speed);
void Car_TurnRight(uint8_t speed_diff, uint8_t base_speed);
void Car_SpinLeft(uint8_t speed);
void Car_SpinRight(uint8_t speed);

#endif
