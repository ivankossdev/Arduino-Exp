#include "telemetry.h"
#include "config.h"
#include "display_log.h"
#include <string.h>
#include <stdlib.h>

// Функция подсчета XOR для проверки
static uint8_t calculateChecksum(const char *str, int len) {
  uint8_t cs = 0;
  for (int i = 0; i < len; i++) {
    cs ^= (uint8_t)str[i];
  }
  return cs;
}

bool telemetryParse(const char *line, Telemetry &out) {
  out.hasTemp = false;
  out.hasAdc  = false;
  out.temp    = 0.0f;
  out.adc     = 0;

  // 1. Ищем звездочку (разделитель контрольной суммы)
  const char *star = strrchr(line, '*');
  if (!star) return false; // Если нет звездочки — кадр не по протоколу

  // 2. Вычисляем длину полезной нагрузки
  int payloadLen = star - line;
  
  // 3. Извлекаем контрольную сумму из строки (HEX)
  char *end;
  long expectedCs = strtol(star + 1, &end, 16);
  if (end == star + 1) return false; // Не удалось прочитать HEX

  // 4. Считаем реальную сумму и сравниваем
  uint8_t actualCs = calculateChecksum(line, payloadLen);
  if (actualCs != (uint8_t)expectedCs) {
    return false; // Суммы не совпали (данные побились)!
  }

  // 5. Если всё ок, парсим только полезную нагрузку (без учета *XX)
  // Для этого создадим временную копию строки
  char cleanLine[UART_BUF_LEN];
  strncpy(cleanLine, line, payloadLen);
  cleanLine[payloadLen] = '\0';

  const char *p;

  p = strstr(cleanLine, "T=");
  if (p) {
    char *e;
    float v = strtod(p + 2, &e);
    if (e != p + 2) {
      out.temp    = v;
      out.hasTemp = true;
    }
  }

  p = strstr(cleanLine, "A=");
  if (p) {
    char *e;
    long v = strtol(p + 2, &e, 10);
    if (e != p + 2) {
      out.adc    = (int)v;
      out.hasAdc = true;
    }
  }

  return out.hasTemp || out.hasAdc;
}

void telemetryHandle(const char *line) {
  Telemetry t;

  if (!telemetryParse(line, t)) {
    // Если парсинг не удался или CRC не сошлась
    char msg[UART_BUF_LEN + 16];
    snprintf(msg, sizeof(msg), "[ERR] bad frame");
    logLine(msg);
    return;
  }

  // Успешный парсинг -> шлем в статусную строку
  char msg[32];
  if (t.hasTemp && t.hasAdc) {
    snprintf(msg, sizeof(msg), "T:%.1fC  A0:%d", t.temp, t.adc);
  } else if (t.hasTemp) {
    snprintf(msg, sizeof(msg), "T:%.1fC", t.temp);
  } else {
    snprintf(msg, sizeof(msg), "A0:%d", t.adc);
  }
  statusLine(msg);
}