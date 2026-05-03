#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
void Motor_L_Setspeed(int8_t speed);
void Motor_R_Setspeed(int8_t speed);
void Car_Brake(void);
void Car_Stop(void);

#endif
