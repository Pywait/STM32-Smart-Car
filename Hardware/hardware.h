#ifndef __HARDWARE_H
#define __HARDWARE_H

#define LED_PORT GPIOB
#define LED_PIN GPIO_Pin_14
#define ISD_PORT GPIOA
#define ISD_PIN GPIO_Pin_8
#define KEY_PORT GPIOA
#define KEY_PIN1 GPIO_Pin_11
#define KEY_PIN2 GPIO_Pin_12
#define BUZZER_PORT GPIOB
#define BUZZER_PIN GPIO_Pin_15

#define ULTRASONIC_PORT GPIOB
#define ULTRASONIC_TX_PIN GPIO_Pin_10
#define ULTRASONIC_RX_PIN GPIO_Pin_11
#define PIR_PORT GPIOB
#define PIR_FRONT_PIN GPIO_Pin_13
#define PIR_BACK_PIN GPIO_Pin_12

// 从左到右物理布局：PB5(最左) PB6(左中) PB7(中间) PB8(右中) PB9(最右)
// 权重分配：-2, -1, 0, 1, 2
#define TRACK_PORT GPIOB
#define TRACK_L1_PIN GPIO_Pin_5     //最左
#define TRACK_L2_PIN GPIO_Pin_6     //左中
#define TRACK_M_PIN  GPIO_Pin_7     //中间
#define TRACK_R2_PIN GPIO_Pin_8     //右中
#define TRACK_R1_PIN GPIO_Pin_9     //最右
#define POSITION_PORT GPIOA
#define POSITION_PIN GPIO_Pin_1

#define MOTOR_L_PWM_A_PIN GPIO_Pin_6	//A6
#define MOTOR_R_PWM_A_PIN GPIO_Pin_7	//A7
#define MOTOR_L1_A_PIN GPIO_Pin_4		//A4
#define MOTOR_L2_A_PIN GPIO_Pin_5		//A5
#define MOTOR_R1_B_PIN GPIO_Pin_0		//B0
#define MOTOR_R2_B_PIN GPIO_Pin_1		//B1

#define ESP_PORT GPIOA
#define ESP_USART1_TX_PIN GPIO_Pin_9
#define ESP_USART1_RX_PIN GPIO_Pin_10

#endif
