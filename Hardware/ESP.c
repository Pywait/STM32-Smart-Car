#include "stm32f10x.h"
#include "hardware.h"
#include "Delay.h"
#include "StateMachine.h"
#include "Motor.h"
#include <string.h>
#include <stdio.h>

/* ── 环形缓冲区 ────────────────────────────────── */

#define RX_BUF_SIZE 256
static volatile uint8_t rx_buffer[RX_BUF_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

#define LINE_BUF_SIZE 128
static char line_buf[LINE_BUF_SIZE];
static uint16_t line_pos = 0;

/* ── 巴法云参数 ────────────────────────────────── */

#define BEMFA_UID   "641dba44ecd7004fce71e73028c876a1"
#define BEMFA_TOPIC "car"
#define BEMFA_HOST  "bemfa.com"
#define BEMFA_PORT  8344

#define HEARTBEAT_INTERVAL 60000
static uint32_t last_heartbeat = 0;

/* ── 环形缓冲区操作 ─────────────────────────────── */

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

static void ESP_SendAT(const char* cmd)
{
	ESP_SendString((uint8_t*)cmd);
	ESP_SendString((uint8_t*)"\r\n");
}

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

static void ESP_FlushRX(void)
{
	uint8_t d;
	while (rx_buffer_get(&d));
}

/**
 * AT+CIPSEND 发送数据:
 *   发送 AT+CIPSEND=<len> → 等待 ">" → 发送数据 → 等待 "SEND OK"
 */
static uint8_t ESP_SendTCP(const char* data)
{
	uint16_t len = strlen(data);
	char cmd[32];
	uint32_t start;
	uint8_t byte, got_prompt;

	sprintf(cmd, "AT+CIPSEND=%d", len);

	ESP_FlushRX();
	ESP_SendAT(cmd);

	/* 等待 '>' 提示符 */
	start = Delay_GetTick();
	got_prompt = 0;
	while (Delay_GetTick() - start < 3000) {
		if (rx_buffer_get(&byte)) {
			if (byte == '>') { got_prompt = 1; break; }
		}
	}
	if (!got_prompt) return 0;

	ESP_SendString((uint8_t*)data);
	return ESP_WaitResp("SEND OK", 3000);
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
	last_heartbeat = 0;

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART1, ENABLE);
}

/* ── 连接巴法云（替代原 MQTT 初始化） ──────────── */

void ESP_MQTTInit(void)
{
	Delay_ms(1000);
	ESP_FlushRX();

	/* 1) 建立 TCP 连接 */
	ESP_SendAT("AT+CIPSTART=\"TCP\",\"bemfa.com\",8344");
	if (!ESP_WaitResp("CONNECT", 5000)) {
		/* 重试一次 */
		ESP_FlushRX();
		ESP_SendAT("AT+CIPSTART=\"TCP\",\"bemfa.com\",8344");
		ESP_WaitResp("CONNECT", 5000);
	}
	Delay_ms(500);

	/* 2) 订阅 car/control 主题 */
	{
		char sub[128];
		sprintf(sub, "cmd=1&uid=%s&topic=%s/control", BEMFA_UID, BEMFA_TOPIC);
		ESP_SendTCP(sub);
		Delay_ms(200);
	}

	last_heartbeat = Delay_GetTick();
}

/* ── 心跳 ───────────────────────────────────────── */

static void ESP_SendHeartbeat(void)
{
	ESP_SendTCP("cmd=0&msg=ping");
}

/* ── 数据上报 ───────────────────────────────────── */

void ESP_SendAlarm(uint8_t type)
{
	char msg[128];
	switch (type) {
	case 0x01:
		sprintf(msg, "cmd=2&uid=%s&topic=%s/alarm&msg=person", BEMFA_UID, BEMFA_TOPIC);
		break;
	case 0x02:
		sprintf(msg, "cmd=2&uid=%s&topic=%s/alarm&msg=obstacle", BEMFA_UID, BEMFA_TOPIC);
		break;
	case 0x03:
		sprintf(msg, "cmd=2&uid=%s&topic=%s/alarm&msg=no obstacle", BEMFA_UID, BEMFA_TOPIC);
		break;
	case 0x04:
		sprintf(msg, "cmd=2&uid=%s&topic=%s/alarm&msg=no person", BEMFA_UID, BEMFA_TOPIC);
		break;
	}
	ESP_SendTCP(msg);
}

void ESP_SendPosition(uint8_t idx)
{
	/* README 格式: msg=x/x+1 （表示小车在 x 与 x+1 之间） */
	char msg[128];
	sprintf(msg, "cmd=2&uid=%s&topic=%s/pos&msg=%d/%d", BEMFA_UID, BEMFA_TOPIC, idx, idx + 1);
	ESP_SendTCP(msg);
}

void ESP_SendBattery(uint8_t level)
{
	char msg[128];
	sprintf(msg, "cmd=2&uid=%s&topic=%s/battery&msg=%d", BEMFA_UID, BEMFA_TOPIC, level);
	ESP_SendTCP(msg);
}

/* ── 下行命令解析 ───────────────────────────────── */

static void ParseBemfaMsg(const char* msg)
{
	/* msg 格式: msg=forward / back / left / right / stop */
	const char* p = strstr(msg, "msg=");
	if (!p) return;
	p += 4; /* 跳过 "msg=" */

	/* 切换到遥控状态 */
	if (current_state != STATE_REMOTE_CONTROL) {
		current_state = STATE_REMOTE_CONTROL;
	}

	if      (strcmp(p, "forward") == 0) { Motor_L_Setspeed(80);  Motor_R_Setspeed(80);  RemoteCtrl_SetCmd('F'); }
	else if (strcmp(p, "back")    == 0) { Motor_L_Setspeed(-80); Motor_R_Setspeed(-80); RemoteCtrl_SetCmd('B'); }
	else if (strcmp(p, "left")    == 0) { Motor_L_Setspeed(-80); Motor_R_Setspeed(80);  RemoteCtrl_SetCmd('L'); }
	else if (strcmp(p, "right")   == 0) { Motor_L_Setspeed(80);  Motor_R_Setspeed(-80); RemoteCtrl_SetCmd('R'); }
	else if (strcmp(p, "stop")    == 0) { Motor_L_Setspeed(0);   Motor_R_Setspeed(0);    RemoteCtrl_SetCmd('S'); }
}

/* 主循环调用: 从环形缓冲取字节拼行 + 心跳维持 */
void ESP_ProcessCommands(void)
{
	uint8_t byte;
	while (rx_buffer_get(&byte)) {
		if (byte == '\n') {
			line_buf[line_pos] = '\0';
			if (line_pos > 0) {
				/* 查找 +IPD,<len>:<data> 格式的 TCP 下行数据 */
				char* ipd = strstr(line_buf, "+IPD,");
				if (ipd) {
					char* colon = strchr(ipd, ':');
					if (colon) {
						ParseBemfaMsg(colon + 1);
					}
				}
			}
			line_pos = 0;
		} else if (byte != '\r') {
			if (line_pos < LINE_BUF_SIZE - 1) {
				line_buf[line_pos++] = (char)byte;
			}
		}
	}

	/* 心跳维持：每 60s 发送 cmd=0&msg=ping */
	uint32_t now = Delay_GetTick();
	if (last_heartbeat > 0 && now - last_heartbeat >= HEARTBEAT_INTERVAL) {
		last_heartbeat = now;
		ESP_SendHeartbeat();
	}
}
