#include "telemetry.h"
#include "config.h"
#include "display_log.h"
#include "uart_watchdog.h"
#include "uart_sender.h"
#include "stats.h"
#include <string.h>
#include <stdlib.h>

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

bool telemetryParse(const char *line, Telemetry &out) {
  out.hasTemp = false;
  out.hasAdc = false;
  out.hasSeq = false;
  out.temp = 0.0f;
  out.adc = 0;
  out.seq = 0;

  const char *star = strrchr(line, '*');
  if (!star) return false;

  int payloadLen = star - line;
  char *end;
  long expectedCs = strtol(star + 1, &end, 16);
  if (end == star + 1) return false;

  if (calculateCRC8(line, payloadLen) != (uint8_t)expectedCs) {
    return false;
  }

  char cleanLine[UART_BUF_LEN];
  strncpy(cleanLine, line, payloadLen);
  cleanLine[payloadLen] = '\0';

  const char *p;

  p = strstr(cleanLine, "S=");
  if (p) {
    char *e;
    long v = strtol(p + 2, &e, 10);
    if (e != p + 2) {
      out.seq = (unsigned int)v;
      out.hasSeq = true;
    }
  }

  p = strstr(cleanLine, "T=");
  if (p) {
    char *e;
    float v = strtod(p + 2, &e);
    if (e != p + 2) {
      out.temp = v;
      out.hasTemp = true;
    }
  }

  p = strstr(cleanLine, "A=");
  if (p) {
    char *e;
    long v = strtol(p + 2, &e, 10);
    if (e != p + 2) {
      out.adc = (int)v;
      out.hasAdc = true;
    }
  }

  return true;
}

void telemetryHandle(const char *line) {
  Telemetry t;
  if (!telemetryParse(line, t)) {
    logLine("[ERR] bad frame");
    return;
  }

  uartWatchdogFeed();

  if (strncmp(line, "ACK:", 4) == 0) {
    char clean[32];
    const char *st = strrchr(line, '*');
    int l = st ? (st - line) : strlen(line);
    strncpy(clean, line, l);
    clean[l] = '\0';

    char msg[64];
    snprintf(msg, sizeof(msg), "[RX] %s", clean);
    logLine(msg);

    // Очищаем ЛЮБУЮ ожидающую команду при получении ACK
    uartClearRetry("");
    return;
  }

  if (strncmp(line, "STAT:", 5) == 0) {
    char msg[64];
    const char *st = strrchr(line, '*');
    int l = st ? (st - line) : strlen(line);
    strncpy(msg, line, l);
    msg[l] = '\0';

    logLine(msg);
    uartClearRetry("");  // Очищаем ожидание
    return;
  }

  if (strncmp(line, "EVT:", 4) == 0) {
    char clean[32];
    const char *st = strrchr(line, '*');
    int l = st ? (st - line) : strlen(line);
    strncpy(clean, line, l);
    clean[l] = '\0';

    char msg[64];
    snprintf(msg, sizeof(msg), "[!!!] %s", clean);
    logLine(msg);  // Выведет "[!!!] EVT:HOT"

    uartClearRetry("");  // На случай, если событие пришло в ответ на команду
    return;
  }

  static unsigned int lastSeq = 0;
  static bool firstSeq = true;

  static float lastTemp = 0.0f;
  static int lastAdc = 0;

  if (t.hasSeq) {
    if (!firstSeq) {
      if (t.seq != lastSeq + 1 && t.seq > lastSeq) {
        uint32_t lost = t.seq - lastSeq - 1;
        g_lostPackets += lost;

        char msg[32];
        snprintf(msg, sizeof(msg), "[!] lost %u pkt", lost);
        logLine(msg);
      }
    }
    lastSeq = t.seq;
    firstSeq = false;
  }

  if (t.hasTemp) lastTemp = t.temp;
  if (t.hasAdc) lastAdc = t.adc;

  char msg[32];
  snprintf(msg, sizeof(msg), "T:%.1f A:%d L:%lu R:%lu", lastTemp, lastAdc, g_lostPackets, g_retries);
  statusLine(msg);
}