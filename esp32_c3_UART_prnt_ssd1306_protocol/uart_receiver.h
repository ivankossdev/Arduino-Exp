#pragma once
#include <Arduino.h>

typedef void (*UartLineCallback)(const char *line);

void uartInit(uint32_t baud, int rx, int tx, UartLineCallback cb);
void uartPoll();

// Возвращает millis() последнего принятого байта
// (или момент uartInit(), если байт ещё не было)
unsigned long uartLastActivity();