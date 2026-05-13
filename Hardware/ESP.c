#include "stm32f10x.h"
#include "hardware.h"
#include "Delay.h"
#include "StateMachine.h"
#include "Motor.h"
#include <string.h>
#include <stdio.h>

// 环形缓冲区
#define RX_BUF_SIZE 256
static volatile uint8_t rx_buffer[RX_BUF_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

// 行缓冲区: 用于解析 ESP 异步消息
#define LINE_BUF_SIZE 128
static char line_buf[LINE_BUF_SIZE];
static uint16_t line_pos = 0;

// MQTT 设备参数 (OneNET)
#define MQTT_PROD_ID  "0G3y0rg843"
#define MQTT_DEV_NAME "Car2"
#define MQTT_AUTH_INFO "version=2018-10-31&res=products%2F0G3y0rg843%2Fdevices%2FCar2&et=1779692492&method=md5&sign=YAUHO4QfJK2gwL9xtDQkCQ%3D%3D"
#define MQTT_BROKER   "mqtts.heclouds.com"
#define MQTT_PORT     1883

/* ── 环形缓冲区 ────────────────────────────────── */

static void rx_buffer_put(uint8_t data)
{
	uint16_t next = (rx_head + 1) % RX_BUF_SIZE;
	if (next != rx_tail) {
		rx_buffer[rx_head] = data;
		rx_head = next;
	}
}

static uint8_t rx_buffer_get(uint8_t *data)
{
	if (rx_head == rx_tail) return 0;
	*data = rx_buffer[rx_tail];
	rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
	return 1;
}

/* ── 发送基础函数 ──────────────────────────────── */

void ESP_SendByte(uint8_t data)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, data);
}

void ESP_SendString(uint8_t* str)
{
	while (*str) ESP_SendByte(*str++);
}

void ESP_SendBytes(uint8_t* data, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++) ESP_SendByte(data[i]);
}

/* 发送 AT 指令并追加 \r\n */
static void ESP_SendAT(const char* cmd)
{
	ESP_SendString((uint8_t*)cmd);
	ESP_SendString((uint8_t*)"\r\n");
}

/* 等待指定字符串出现在环形缓冲区，超时返回 0 */
static uint8_t ESP_WaitResp(const char* expect, uint32_t timeout_ms)
{
	uint32_t start = Delay_GetTick();
	uint8_t data;
	uint16_t match_idx = 0;
	uint16_t len = strlen(expect);

	while (Delay_GetTick() - start < timeout_ms) {
		if (rx_buffer_get(&data)) {
			if ((char)data == expect[match_idx]) {
				match_idx++;
				if (match_idx >= len) return 1;
			} else {
				match_idx = 0;
			}
		}
	}
	return 0;
}

/* 清空环形缓冲区 (丢弃残留数据) */
static void ESP_FlushRX(void)
{
	uint8_t d;
	while (rx_buffer_get(&d));
}

/* ── USART1 中断 ───────────────────────────────── */

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		uint8_t data = USART_ReceiveData(USART1);
		rx_buffer_put(data);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

/* ── 串口初始化 ────────────────────────────────── */

void ESP_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = ESP_USART1_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = ESP_USART1_RX_PIN;
	GPIO_Init(ESP_PORT, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);

	rx_head = 0;
	rx_tail = 0;
	line_pos = 0;

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART1, ENABLE);
}

/* ── MQTT 初始化序列 ────────────────────────────── */

void ESP_MQTTInit(void)
{
	// 等待 ESP-01S 启动
	Delay_ms(1000);
	ESP_FlushRX();

	// 1) 配置 MQTT 用户属性
	ESP_SendAT("AT+MQTTUSERCFG=0,1,\"Car2\",\"0G3y0rg843\",\""
		"version=2018-10-31&res=products%2F0G3y0rg843%2Fdevices%2FCar2"
		"&et=1779692492&method=md5&sign=YAUHO4QfJK2gwL9xtDQkCQ%3D%3D\",0,0");
	if (!ESP_WaitResp("OK", 3000)) {
		ESP_FlushRX();
	}
	Delay_ms(200);

	// 2) 连接 MQTT Broker
	ESP_SendAT("AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1");
	if (!ESP_WaitResp("OK", 5000)) {
		ESP_FlushRX();
	}
	Delay_ms(200);

	// 3) 订阅平台调用设备服务 — RemoteCtrl
	ESP_SendAT("AT+MQTTSUB=0,\"$sys/0G3y0rg843/Car2/thing/service/RemoteCtrl/invoke\",1");
	ESP_WaitResp("OK", 3000);
	Delay_ms(200);

	// 4) 订阅平台调用设备服务 — PatrolMode
	ESP_SendAT("AT+MQTTSUB=0,\"$sys/0G3y0rg843/Car2/thing/service/PatrolMode/invoke\",1");
	ESP_WaitResp("OK", 3000);
	Delay_ms(200);
}

/* ── 数据上报 ───────────────────────────────────── */

// 报警事件上报 (0x01=人员, 0x02=障碍物)
void ESP_SendAlarm(uint8_t type)
{
	if (type == 0x01) {
		ESP_SendAT("AT+MQTTPUB=0,\"$sys/0G3y0rg843/Car2/thing/event/post\","
			"\"{\\\"id\\\":\\\"124\\\"\\,\\\"params\\\":{\\\"AlarmPerson\\\""
			":{\\\"value\\\":{\\\"info\\\":\\\"异常人员闯入\\\"}}}}\",0,0");
	} else {
		ESP_SendAT("AT+MQTTPUB=0,\"$sys/0G3y0rg843/Car2/thing/event/post\","
			"\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"AlarmObstacle\\\""
			":{\\\"value\\\":{\\\"info\\\":\\\"前方障碍物\\\"}}}}\",0,0");
	}
}

// 位置属性上报 — idx 当前标号, total 总标号数
void ESP_SendPosition(uint8_t idx, uint8_t total)
{
	ESP_SendString("AT+MQTTPUB=0,\"$sys/0G3y0rg843/Car2/thing/property/post\","
		"\"{\\\"id\\\":\\\"125\\\"\\,\\\"params\\\":{\\\"Location\\\""
		":{\\\"value\\\":\\\"小车在");
	ESP_SendByte('0' + idx);
	ESP_SendString("与");
	ESP_SendByte('0' + total);
	ESP_SendString("之间\\\"}}}\",0,0\r\n");
}

// 电量属性上报
void ESP_SendBattery(uint8_t level)
{
	char num[4];
	sprintf(num, "%d", level);
	ESP_SendString("AT+MQTTPUB=0,\"$sys/0G3y0rg843/Car2/thing/property/post\","
		"\"{\\\"id\\\":\\\"126\\\"\\,\\\"params\\\":{\\\"Battery\\\""
		":{\\\"value\\\":");
	ESP_SendString((uint8_t*)num);
	ESP_SendString("}}}\",0,0\r\n");
}

/* ── 下行命令解析 (MQTTSUB) ────────────────────── */

/* 在 line 中查找 JSON 字段值: "key":"value" 或 "key":num */
static void ParseMQTTLine(const char* line)
{
	const char* p;

	// === 遥控命令 RemoteCtrl: "cmd":"F"/"B"/"L"/"R"/"S" ===
	p = strstr(line, "\"cmd\"");
	if (p) {
		// 切换到遥控状态 (如果不在该状态)
		if (current_state != STATE_REMOTE_CONTROL) {
			current_mode = MODE_TRACKING;   // 占位, 实际为遥控
			current_state = STATE_REMOTE_CONTROL;
		}

		if      (strstr(p, "\"F\"")) { Motor_L_Setspeed(80); Motor_R_Setspeed(80); }
		else if (strstr(p, "\"B\"")) { Motor_L_Setspeed(-80); Motor_R_Setspeed(-80); }
		else if (strstr(p, "\"L\"")) { Motor_L_Setspeed(-80); Motor_R_Setspeed(80); }
		else if (strstr(p, "\"R\"")) { Motor_L_Setspeed(80); Motor_R_Setspeed(-80); }
		else if (strstr(p, "\"S\"")) { Motor_L_Setspeed(0); Motor_R_Setspeed(0); }
		return;
	}

	// === 巡逻模式 PatrolMode: "mode":0/1/2 ===
	p = strstr(line, "\"mode\"");
	if (p) {
		if      (strstr(p, "\"mode\":0")) { current_mode = MODE_TRACKING;           current_state = STATE_TRACKING; }
		else if (strstr(p, "\"mode\":1")) { current_mode = MODE_OBSTACLE_AVOIDANCE; current_state = STATE_OBSTACLE_AVOIDANCE; }
		else if (strstr(p, "\"mode\":2")) { current_state = STATE_REMOTE_CONTROL; }
		return;
	}
}

// 主循环调用: 从环形缓冲取字节拼行, 遇到换行时解析
void ESP_ProcessCommands(void)
{
	uint8_t byte;
	while (rx_buffer_get(&byte)) {
		if (byte == '\n') {
			line_buf[line_pos] = '\0';
			if (line_pos > 0) {
				ParseMQTTLine(line_buf);
			}
			line_pos = 0;
		} else if (byte != '\r') {
			if (line_pos < LINE_BUF_SIZE - 1) {
				line_buf[line_pos++] = (char)byte;
			}
		}
	}
}
