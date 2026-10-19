#include "uart_sender.h"
#include "config.h"
#include "display_log.h"

static uint8_t calculateChecksum(const char *str) {
  uint8_t cs = 0;
  while (*str) cs ^= (uint8_t)*str++;
  return cs;
}

void uartSendRaw(const char *cmd) {
  uint8_t cs = calculateChecksum(cmd);
  Serial1.print(cmd);
  Serial1.print('*');
  if (cs < 16) Serial1.print('0');
  Serial1.print(cs, HEX);
  Serial1.print('\n');
  Serial1.flush();
}

void uartSendLine(const char *cmd) {
  uartSendRaw(cmd);
  char msg[64];
  snprintf(msg, sizeof(msg), "[TX] %s", cmd);
  logLine(msg);
}