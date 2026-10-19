#include "telemetry.h"
#include "config.h"
#include "display_log.h"
#include <string.h>
#include <stdlib.h>

static uint8_t calculateChecksum(const char *str, int len) {
  uint8_t cs = 0;
  for (int i = 0; i < len; i++) cs ^= (uint8_t)str[i];
  return cs;
}

bool telemetryParse(const char *line, Telemetry &out) {
  out.hasTemp = false;
  out.hasAdc  = false;
  out.hasSeq  = false;
  out.temp    = 0.0f;
  out.adc     = 0;
  out.seq     = 0;

  const char *star = strrchr(line, '*');
  if (!star) return false;

  int payloadLen = star - line;
  char *end;
  long expectedCs = strtol(star + 1, &end, 16);
  if (end == star + 1) return false;

  if (calculateChecksum(line, payloadLen) != (uint8_t)expectedCs) return false;

  char cleanLine[UART_BUF_LEN];
  strncpy(cleanLine, line, payloadLen);
  cleanLine[payloadLen] = '\0';

  const char *p;

  p = strstr(cleanLine, "S=");
  if (p) {
    char *e;
    long v = strtol(p + 2, &e, 10);
    if (e != p + 2) { out.seq = (unsigned int)v; out.hasSeq = true; }
  }

  p = strstr(cleanLine, "T=");
  if (p) {
    char *e;
    float v = strtod(p + 2, &e);
    if (e != p + 2) { out.temp = v; out.hasTemp = true; }
  }

  p = strstr(cleanLine, "A=");
  if (p) {
    char *e;
    long v = strtol(p + 2, &e, 10);
    if (e != p + 2) { out.adc = (int)v; out.hasAdc = true; }
  }

  return out.hasTemp || out.hasAdc || out.hasSeq;
}

void telemetryHandle(const char *line) {
  // Обработка ответов (ACK) от Uno
  if (strncmp(line, "ACK:", 4) == 0) {
    char clean[32];
    const char *st = strrchr(line, '*');
    int l = st ? (st - line) : strlen(line);
    strncpy(clean, line, l);
    clean[l] = '\0';
    
    char msg[64];
    snprintf(msg, sizeof(msg), "[RX] %s", clean);
    logLine(msg);
    return;
  }

  Telemetry t;
  if (!telemetryParse(line, t)) {
    logLine("[ERR] bad frame");
    return;
  }

  // Проверка пропущенных пакетов (Sequence)
  static unsigned int lastSeq = 0;
  static bool firstSeq = true;

  if (t.hasSeq) {
    if (!firstSeq) {
      // Если пришел не следующий по порядку пакет (и это не переполнение)
      if (t.seq != lastSeq + 1 && t.seq > lastSeq) {
        char msg[32];
        snprintf(msg, sizeof(msg), "[!] lost %u pkt", t.seq - lastSeq - 1);
        logLine(msg);
      }
    }
    lastSeq = t.seq;
    firstSeq = false;
  }

  // Обновление статуса
  char msg[32];
  if (t.hasTemp && t.hasAdc) {
    snprintf(msg, sizeof(msg), "T:%.1fC  A0:%d", t.temp, t.adc);
  } else if (t.hasTemp) {
    snprintf(msg, sizeof(msg), "T:%.1fC", t.temp);
  } else if (t.hasAdc) {
    snprintf(msg, sizeof(msg), "A0:%d", t.adc);
  }
  statusLine(msg);
}