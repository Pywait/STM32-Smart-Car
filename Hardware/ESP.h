#ifndef __ESP_H
#define __ESP_H

void ESP_Init(void);
void ESP_MQTTInit(void);
void ESP_SendString(uint8_t* str);
void ESP_SendByte(uint8_t data);
void ESP_SendBytes(uint8_t* data, uint16_t len);

// 上报函数
void ESP_SendAlarm(uint8_t type);       // type: 0x01 人员闯入, 0x02 障碍物
void ESP_SendPosition(uint8_t idx, uint8_t total);  // idx: 当前点(1~total), total: 总点数
void ESP_SendBattery(uint8_t level);

// 主循环中频繁调用，解析APP指令
void ESP_ProcessCommands(void);

#endif
