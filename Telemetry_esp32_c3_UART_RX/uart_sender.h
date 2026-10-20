#pragma once
#include <Arduino.h>

// Отправить строку + '\n' во вторую плату.
// Дублирует отправленное в лог как "[TX] <cmd>".
void uartSendLine(const char *cmd);

// Без лога (на случай служебных сообщений)
void uartSendRaw(const char *cmd);