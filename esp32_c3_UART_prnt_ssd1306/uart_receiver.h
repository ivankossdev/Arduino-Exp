#pragma once
#include <Arduino.h>

// Колбэк получает готовую строку (без '\n')
typedef void (*UartLineCallback)(const char *line);

void uartInit(uint32_t baud, int rx, int tx, UartLineCallback cb);
void uartPoll();