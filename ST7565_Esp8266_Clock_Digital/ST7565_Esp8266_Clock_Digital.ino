#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include "ds3231.h"
#include "serialSet.h"
#include "display.h"

// ===== Timers =====
unsigned long lastColonToggle = 0;
unsigned long lastRTCPoll     = 0;
const unsigned long COLON_BLINK_MS = 500;
const unsigned long RTC_POLL_MS    = 100;

// ===== Cached display state =====
int  prevHours    = -1;
int  prevMinutes  = -1;
int  prevSeconds  = -1;
int  prevDate     = -1;
int  prevMonth    = -1;
int  prevYear     = -1;
char prevTemp[8]  = "";
char prevDay[8]   = "";
bool colonVisible = true;
bool needRedraw   = true;

// ===== Devices =====
Display    display;
SystemTime sysTime(DS3231);
SerialSet  serialSet(115200, sysTime);

void setup() {
  Wire.begin();
  display.begin();
  serialSet.Init();

  pollRTC();
  lastRTCPoll     = millis();
  lastColonToggle = millis();

  // Включаем hardware watchdog: 8-секундный таймаут.
  // Если loop() где-то зависнет (например, I2C-шина), плата перезагрузится.
  ESP.wdtEnable(WDTO_8S);
}

void loop() {
  ESP.wdtFeed();  // сбрасываем 8-секундный счётчик

  unsigned long currentMillis = millis();
  serialSet.SetCMD();

  if (currentMillis - lastRTCPoll >= RTC_POLL_MS) {
    lastRTCPoll = currentMillis;
    pollRTC();
  }

  if (currentMillis - lastColonToggle >= COLON_BLINK_MS) {
    lastColonToggle = currentMillis;
    colonVisible = !colonVisible;
    needRedraw = true;
  }

  if (needRedraw) {
    needRedraw = false;
    display.draw(prevHours, prevMinutes, prevSeconds,
                 prevYear, prevMonth, prevDate,
                 colonVisible,
                 prevTemp,
                 prevDay);
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
  const char* t   = sysTime.getTemperature();
  const char* day = sysTime.getDay();

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

  if (strcmp(day, prevDay) != 0) {
    strncpy(prevDay, day, sizeof(prevDay) - 1);
    prevDay[sizeof(prevDay) - 1] = '\0';
    changed = true;
  }

  if (changed) needRedraw = true;
}