#pragma once
#include <Arduino.h>

extern uint32_t g_lostPackets;
extern uint32_t g_retries;

void statsInit();