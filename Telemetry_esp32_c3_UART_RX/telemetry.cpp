#include "telemetry.h"
#include "config.h"
#include "display_log.h"
#include "uart_watchdog.h"
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
  if (!star) return false; // Нет контрольной суммы — брак

  int payloadLen = star - line;
  char *end;
  long expectedCs = strtol(star + 1, &end, 16);
  if (end == star + 1) return false; // Не смогли прочитать HEX

  if (calculateChecksum(line, payloadLen) != (uint8_t)expectedCs) {
    return false; // CRC не сошлось — данные побились
  }

  // --- ЕСЛИ ДОШЛИ ДО СЮДА, ПАКЕТ ЦЕЛЫЙ ---
  
  char cleanLine[UART_BUF_LEN];
  strncpy(cleanLine, line, payloadLen);
  cleanLine[payloadLen] = '\0';

  const char *p;

  p = strstr(cleanLine, "S=");
  if (p) {
    char *e; long v = strtol(p + 2, &e, 10);
    if (e != p + 2) { out.seq = (unsigned int)v; out.hasSeq = true; }
  }

  p = strstr(cleanLine, "T=");
  if (p) {
    char *e; float v = strtod(p + 2, &e);
    if (e != p + 2) { out.temp = v; out.hasTemp = true; }
  }

  p = strstr(cleanLine, "A=");
  if (p) {
    char *e; long v = strtol(p + 2, &e, 10);
    if (e != p + 2) { out.adc = (int)v; out.hasAdc = true; }
  }

  // ВАЖНО: Возвращаем true, так как CRC сошлась (даже если это просто ACK)
  return true; 
}

void telemetryHandle(const char *line) {
  // 1. Проверяем CRC
  Telemetry t;
  if (!telemetryParse(line, t)) {
    logLine("[ERR] bad frame");
    return;
  }

  // 2. Пакет валидный! Кормим watchdog
  uartWatchdogFeed();

  // 3. Обработка ответов (ACK)
  if (strncmp(line, "ACK:", 4) == 0) {
    char clean[32];
    const char *st = strrchr(line, '*');
    int l = st ? (st - line) : strlen(line);
    strncpy(clean, line, l);
    clean[l] = '\0';
    
    char msg[64];
    snprintf(msg, sizeof(msg), "[RX] %s", clean);
    logLine(msg);
    return; // Выходим, чтобы не обновлять телеметрию
  }

  // 4. Обработка статуса (STAT)
  if (strncmp(line, "STAT:", 5) == 0) {
    char msg[64];
    const char *st = strrchr(line, '*');
    int l = st ? (st - line) : strlen(line);
    strncpy(msg, line, l);
    msg[l] = '\0';
    
    logLine(msg);
    return; // Выходим, чтобы не обновлять телеметрию
  }

  // 5. Проверка пропущенных пакетов (Sequence)
  static unsigned int lastSeq = 0;
  static bool firstSeq = true;

  if (t.hasSeq) {
    if (!firstSeq) {
      if (t.seq != lastSeq + 1 && t.seq > lastSeq) {
        char msg[32];
        snprintf(msg, sizeof(msg), "[!] lost %u pkt", t.seq - lastSeq - 1);
        logLine(msg);
      }
    }
    lastSeq = t.seq;
    firstSeq = false;
  }

  // 6. Обновление статуса (только если это реально телеметрия)
  char msg[32];
  if (t.hasTemp && t.hasAdc) {
    snprintf(msg, sizeof(msg), "T:%.2fC  A0:%d", t.temp, t.adc); // было %.1f
    statusLine(msg);
  } else if (t.hasTemp) {
    snprintf(msg, sizeof(msg), "T:%.2fC", t.temp); // было %.1f
    statusLine(msg);
  }
}