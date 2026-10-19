// telemetry.h
#pragma once
#include <Arduino.h>

struct Telemetry {
  bool  hasTemp;
  bool  hasAdc;
  float temp;
  int   adc;
};

bool telemetryParse(const char *line, Telemetry &out);
void telemetryHandle(const char *line);