#pragma once
#include <Arduino.h>

void uartSendRaw(const char *cmd);
void uartSendLine(const char *cmd);

// Отправка с гарантией доставки (с повторами)
void uartSendReliable(const char *cmd);

// Вызывать в loop() для проверки таймаутов
void uartRetryPoll();

// Вызывать при получении ACK или STAT, чтобы остановить повторы
void uartClearRetry(const char *cmd);