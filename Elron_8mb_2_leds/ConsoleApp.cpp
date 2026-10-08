#include "Console.h"
#include "Settings.h"

using namespace console_impl;

void Console::cmdStats() {
  unsigned long ms  = millis();
  unsigned long sec = ms / 1000;
  unsigned long min = sec / 60;
  unsigned long hrs = min / 60;

  Serial.println("--- Device stats ---");
  Serial.print("uptime       = ");
  Serial.print(hrs);        Serial.print("h ");
  Serial.print(min % 60);   Serial.print("m ");
  Serial.print(sec % 60);   Serial.print("s (");
  Serial.print(ms);         Serial.println(" ms)");

  Serial.print("bootCount    = "); Serial.println(settings.bootCount);
  Serial.print("buttonCount  = "); Serial.println(settings.buttonCount);

  Serial.print("sizeof(Settings) = ");
  Serial.print(sizeof(Settings));
  Serial.println(" bytes");
}

void Console::cmdLed2(char* args) {
  char* cursor = args;
  char* valStr = parseToken(&cursor);

  if (valStr == nullptr) {
    Serial.print("LED2 interval = ");
    Serial.println(settings.led2Interval);
    return;
  }

  long v = atol(valStr);
  if (v != 0 && (v < 50 || v > 60000)) {
    Serial.println("Range: 0 (off) or 50..60000");
    return;
  }

  settings.led2Interval = (uint16_t)v;
  settingsSave();
  if (onSettingsChanged) onSettingsChanged();

  Serial.print("LED2 interval = ");
  if (v == 0) Serial.println("OFF");
  else        Serial.println(v);
}