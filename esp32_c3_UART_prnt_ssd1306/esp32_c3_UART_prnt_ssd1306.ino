#include "config.h"
#include "display_log.h"
#include "uart_receiver.h"
#include "uart_watchdog.h"
#include "buttons.h"
#include "heartbeat.h"

static void onUartLine(const char *line) {
  char msg[UART_BUF_LEN + 8];
  snprintf(msg, sizeof(msg), "[UART] %s", line);
  logLine(msg);
}

void setup() {
  logInit();
  buttonsInit();
  uartInit(UART_BAUD, UART_RX, UART_TX, onUartLine);
  uartWatchdogInit(UART_TIMEOUT_MS);
  heartbeatInit(HB_PERIOD_MS);

  logLine("System ready");
}

void loop() {
  switch (buttonsPoll()) {
    case BTN1_PRESSED: logClear();         break;
    case BTN2_PRESSED: heartbeatShowNow(); break;
    default: break;
  }

  uartPoll();
  uartWatchdogPoll();
  heartbeatPoll();
}