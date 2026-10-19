#include "uart_watchdog.h"
#include "config.h"
#include "display_log.h"
#include "uart_receiver.h"

static uint32_t timeoutMs  = 10000;
static bool     isSilent   = false;
static unsigned long silentStart = 0;   // время последнего байта ПЕРЕД тишиной

void uartWatchdogInit(uint32_t timeout_ms) {
  timeoutMs   = timeout_ms;
  isSilent    = false;
  silentStart = millis();
}

void uartWatchdogPoll() {
  unsigned long now  = millis();
  unsigned long last = uartLastActivity();
  bool silent = (now - last) > timeoutMs;

  if (silent && !isSilent) {
    // живо -> тишина
    isSilent    = true;
    silentStart = last;
    logLine("[!] UART timeout");
  } else if (!silent && isSilent) {
    // тишина -> живо
    isSilent = false;
    char buf[32];
    snprintf(buf, sizeof(buf), "[!] UART back %lus",
             (now - silentStart) / 1000UL);
    logLine(buf);
  }
}

bool uartWatchdogIsSilent() {
  return isSilent;
}