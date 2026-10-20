#pragma once
#include <Arduino.h>

struct Telemetry {
  bool         hasTemp;
  bool         hasAdc;
  bool         hasSeq;
  float        temp;
  int          adc;
  unsigned int seq;
};

bool telemetryParse(const char *line, Telemetry &out);
void telemetryHandle(const char *line);