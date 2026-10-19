#include "uart_sender.h"
#include "config.h"
#include "display_log.h"

void uartSendRaw(const char *cmd) {
  Serial1.print(cmd);
  Serial1.print('\n');
  Serial1.flush();       // ждём, пока байты уедут в UART
}

void uartSendLine(const char *cmd) {
  uartSendRaw(cmd);

  char msg[64];
  snprintf(msg, sizeof(msg), "[TX] %s", cmd);
  logLine(msg);
}