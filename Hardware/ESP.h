#ifndef __ESP_H
#define __ESP_H

void ESP_Init(void);
void ESP_MQTTInit(void);
void ESP_SendString(uint8_t* str);
void ESP_SendByte(uint8_t data);
void ESP_SendBytes(uint8_t* data, uint16_t len);

// Report functions
void ESP_SendAlarm(uint8_t type);       // type: 0x01 person, 0x02 obstacle, 0x03 no obstacle, 0x04 no person
void ESP_SendPosition(uint8_t idx);  // idx: current marker, sends "msg=idx/(idx+1)"
void ESP_SendBattery(uint8_t level);

// Call frequently in main loop to parse APP commands
void ESP_ProcessCommands(void);

#endif
