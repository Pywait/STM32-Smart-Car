#ifndef __HARDWARE_H
#define __HARDWARE_H

#define usart2_GPIO_PORT GPIOA
#define usart2_USART_PORT USART2
#define usart2_TX_PIN GPIO_Pin_2
#define usart2_RX_PIN GPIO_Pin_3

#define LED_PORT GPIOB
#define LED_PIN GPIO_Pin_14
#define ISD_PORT GPIOA
#define ISD_PIN GPIO_Pin_8
#define KEY_PORT GPIOA
#define KEY_PIN1 GPIO_Pin_2
#define KEY_PIN2 GPIO_Pin_3

#define BUZZER_PORT GPIOB
#define BUZZER_PIN GPIO_Pin_15

#define MOTOR_L_PWM_PIN_A GPIO_Pin_6	//A6
#define MOTOR_R_PWM_PIN_A GPIO_Pin_7	//A7
#define MOTOR_L1_PIN_A GPIO_Pin_4		//A4
#define MOTOR_L2_PIN_A GPIO_Pin_5		//A5
#define MOTOR_R1_PIN_B GPIO_Pin_0		//B0
#define MOTOR_R2_PIN_B GPIO_Pin_1		//B1

#define TRACK_PORT GPIOB
#define TRACK_L1_PIN GPIO_Pin_5
#define TRACK_L2_PIN GPIO_Pin_6
#define TRACK_R1_PIN GPIO_Pin_7
#define TRACK_R2_PIN GPIO_Pin_8
#define TRACK_M_PIN GPIO_Pin_9

#define ULTRASONIC_PORT GPIOB
#define ULTRASONIC_TX_PIN GPIO_Pin_10
#define ULTRASONIC_RX_PIN GPIO_Pin_11

#define PIR_PORT GPIOB
#define PIR_RCC_PORT RCC_APB2Periph_GPIOB
#define PIR_FRONT_PIN GPIO_Pin_13
#define PIR_BACK_PIN GPIO_Pin_12

#define POSITION_PORT GPIOA
#define POSITION_PIN GPIO_Pin_1

#define ESP_PORT GPIOA
#define ESP_USART1_TX_PIN GPIO_Pin_9
#define ESP_USART1_RX_PIN GPIO_Pin_10
#define ESP_GPIO0_PIN GPIO_Pin_12
#define ESP_GPIO2_PIN GPIO_Pin_15
#define ESP_RST_PIN GPIO_Pin_11

#endif
