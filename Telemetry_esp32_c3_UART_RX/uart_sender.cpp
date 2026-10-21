#include "uart_sender.h"
#include "config.h"
#include "display_log.h"
#include <string.h>

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
  uint8_t cs = calculateCRC8(cmd, strlen(cmd));
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

// --- Логика повторов ---
#define CMD_TIMEOUT_MS 500
#define MAX_RETRIES 3

static char pendingCmd[16] = "";
static int retriesLeft = 0;
static unsigned long lastCmdTime = 0;

void uartSendReliable(const char *cmd) {
  strncpy(pendingCmd, cmd, sizeof(pendingCmd) - 1);
  pendingCmd[sizeof(pendingCmd) - 1] = '\0';
  retriesLeft = MAX_RETRIES;
  lastCmdTime = millis();
  
  uartSendLine(cmd); // Шлем первый раз (с логом)
}

void uartRetryPoll() {
  if (retriesLeft > 0 && (millis() - lastCmdTime >= CMD_TIMEOUT_MS)) {
    retriesLeft--;
    lastCmdTime = millis();

    if (retriesLeft == 0) {
      // Все попытки исчерпаны
      char msg[32];
      snprintf(msg, sizeof(msg), "[!] CMD timeout: %s", pendingCmd);
      logLine(msg);
      pendingCmd[0] = '\0';
    } else {
      // Отправляем повторно (без лога [TX], чтобы не спамить)
      uartSendRaw(pendingCmd); 
      char msg[32];
      snprintf(msg, sizeof(msg), "[TX] retry %s (%d)", pendingCmd, MAX_RETRIES - retriesLeft);
      logLine(msg);
    }
  }
}

void uartClearRetry(const char *cmd) {
  // Если пришел ответ на нашу команду — отменяем повторы
  if (pendingCmd[0] != '\0' && strncmp(pendingCmd, cmd, strlen(pendingCmd)) == 0) {
    retriesLeft = 0;
    pendingCmd[0] = '\0';
  }
}