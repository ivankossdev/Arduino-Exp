#pragma once
#include <Arduino.h>

void heartbeatInit(uint32_t period_ms);
void heartbeatPoll();
void heartbeatShowNow();