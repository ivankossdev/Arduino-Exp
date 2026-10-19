#pragma once
#include <Arduino.h>

void uartWatchdogInit(uint32_t timeout_ms);
void uartWatchdogPoll();

// Состояние: true — сейчас тишина (таймаут активен)
bool uartWatchdogIsSilent();
void uartWatchdogFeed();