#include "heartbeat.h"
#include "display_log.h"
#include <Arduino.h> // Этого достаточно для объекта ESP

static uint32_t period = 10000;
static unsigned long lastHB = 0;

void heartbeatShowNow() {
  unsigned long up_s   = millis() / 1000UL;
  unsigned long hfree  = ESP.getFreeHeap() / 1024UL;
  // Используем ESP.getMaxAllocHeap() для размера наибольшего свободного блока
  unsigned long hlarge = ESP.getMaxAllocHeap() / 1024UL;

  char l1[24];
  char l2[24];
  snprintf(l1, sizeof(l1), "[HB] up=%lus", up_s);
  snprintf(l2, sizeof(l2), "h=%luk lg=%luk", hfree, hlarge);
  logLine(l1);
  logLine(l2);
}

void heartbeatInit(uint32_t period_ms) {
  period = period_ms;
  lastHB = millis();
}

void heartbeatPoll() {
  if (millis() - lastHB >= period) {
    lastHB = millis();
    heartbeatShowNow();
  }
}