#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include "ds3231.h"
#include "serialSet.h"
#include "display.h"

// Таймеры
unsigned long lastColonToggle = 0;
unsigned long lastRTCPoll     = 0;

const unsigned long COLON_BLINK_MS = 500;
const unsigned long RTC_POLL_MS    = 100;

// Кеш последних значений
int  prevHours    = -1;
int  prevMinutes  = -1;
int  prevSeconds  = -1;
int  prevDate     = -1;
int  prevMonth    = -1;
int  prevYear     = -1;
char prevTemp[8]  = "";
bool colonVisible = true;
bool needRedraw   = true;

// Устройства
Display    display;
SystemTime sysTime(DS3231);
At24       eeprom(EEPROM_I2C_ADDRESS);
SerialSet  serialSet(115200, sysTime, eeprom);

void setup() {
  Wire.begin();
  display.begin();
  serialSet.Init();

  pollRTC();
  lastRTCPoll     = millis();
  lastColonToggle = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  serialSet.SetCMD();

  // 1. Опрос RTC не чаще RTC_POLL_MS
  if (currentMillis - lastRTCPoll >= RTC_POLL_MS) {
    lastRTCPoll = currentMillis;
    pollRTC();
  }

  // 2. Мигание двоеточий
  if (currentMillis - lastColonToggle >= COLON_BLINK_MS) {
    lastColonToggle = currentMillis;
    colonVisible = !colonVisible;
    needRedraw = true;
  }

  // 3. Перерисовка только при изменениях
  if (needRedraw) {
    needRedraw = false;
    display.draw(prevHours, prevMinutes, prevSeconds,
                 prevYear, prevMonth, prevDate,
                 colonVisible,
                 prevTemp);
  }
}

void pollRTC() {
  sysTime.getTime();
  int h  = sysTime.getHours();
  int mi = sysTime.getMinutes();
  int s  = sysTime.getSeconds();
  int y  = sysTime.getYear();
  int mo = sysTime.getMonth();
  int d  = sysTime.getDate();

  const char* t = sysTime.getTemperature();

  bool changed = false;

  if (h != prevHours || mi != prevMinutes || s != prevSeconds
      || d != prevDate || mo != prevMonth || y != prevYear) {
    prevHours   = h;
    prevMinutes = mi;
    prevSeconds = s;
    prevDate    = d;
    prevMonth   = mo;
    prevYear    = y;
    changed = true;
  }

  if (strcmp(t, prevTemp) != 0) {
    strncpy(prevTemp, t, sizeof(prevTemp) - 1);
    prevTemp[sizeof(prevTemp) - 1] = '\0';
    changed = true;
  }

  if (changed) needRedraw = true;
}