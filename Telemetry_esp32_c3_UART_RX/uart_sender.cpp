#include "uart_sender.h"
#include "config.h"
#include "display_log.h"

// Новая функция
static uint8_t calculateCRC8(const char *str, int len) {
  uint8_t crc = 0;
  for (int i = 0; i < len; i++) {
    crc ^= (uint8_t)str[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x80) crc = (crc << 1) ^ 0x07;
      else crc <<= 1;
    }
  }
  return crc;
}

void uartSendRaw(const char *cmd) {
  uint8_t cs = calculateCRC8(cmd, strlen(cmd)); // было calculateChecksum
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