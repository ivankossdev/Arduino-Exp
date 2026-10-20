#pragma once
#include <Arduino.h>

void logInit();
void logLine(const char *s);
void logClear();
void statusLine(const char *s);