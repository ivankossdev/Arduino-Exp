#include "uart_watchdog.h"
#include "config.h"
#include "display_log.h"
//#include "uart_receiver.h" // Больше не нужен, слушаем только валидные пакеты

static uint32_t timeoutMs  = 10000;
static bool     isSilent   = false;
static unsigned long lastValidActivity = 0;

void uartWatchdogInit(uint32_t timeout_ms) {
  timeoutMs   = timeout_ms;
  isSilent    = false;
  lastValidActivity = millis();
}

void uartWatchdogFeed() {
  lastValidActivity = millis();
}

void uartWatchdogPoll() {
  unsigned long now  = millis();
  bool silent = (now - lastValidActivity) > timeoutMs;

  if (silent && !isSilent) {
    isSilent    = true;
    logLine("[!] UART timeout");
  } else if (!silent && isSilent) {
    isSilent = false;
    char buf[32];
    snprintf(buf, sizeof(buf), "[!] UART back %lus",
             (now - lastValidActivity) / 1000UL); // Упростили
    logLine(buf);
  }
}

bool uartWatchdogIsSilent() {
  return isSilent;
}