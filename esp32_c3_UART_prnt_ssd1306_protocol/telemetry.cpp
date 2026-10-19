// telemetry.cpp
#include "telemetry.h"
#include "config.h"
#include "display_log.h"
#include <string.h>
#include <stdlib.h>

bool telemetryParse(const char *line, Telemetry &out) {
  out.hasTemp = false;
  out.hasAdc  = false;
  out.temp    = 0.0f;
  out.adc     = 0;

  const char *p;

  p = strstr(line, "T=");
  if (p) {
    char *end;
    float v = strtod(p + 2, &end);
    if (end != p + 2) {
      out.temp    = v;
      out.hasTemp = true;
    }
  }

  p = strstr(line, "A=");
  if (p) {
    char *end;
    long v = strtol(p + 2, &end, 10);
    if (end != p + 2) {
      out.adc    = (int)v;
      out.hasAdc = true;
    }
  }

  return out.hasTemp || out.hasAdc;
}

void telemetryHandle(const char *line) {
  Telemetry t;

  if (!telemetryParse(line, t)) {
    char msg[UART_BUF_LEN + 16];
    snprintf(msg, sizeof(msg), "[ERR] rx: %s", line);
    logLine(msg);
    return;
  }

  char msg[32];
  if (t.hasTemp && t.hasAdc) {
    snprintf(msg, sizeof(msg), "[T] %.1fC A0=%d", t.temp, t.adc);
  } else if (t.hasTemp) {
    snprintf(msg, sizeof(msg), "[T] %.1fC", t.temp);
  } else {
    snprintf(msg, sizeof(msg), "[A] A0=%d", t.adc);
  }
  logLine(msg);
}