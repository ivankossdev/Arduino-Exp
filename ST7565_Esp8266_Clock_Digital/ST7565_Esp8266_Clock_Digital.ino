#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include "ds3231.h"
#include "serialSet.h"
#include "display.h"

// ===== Timers =====
unsigned long lastColonToggle = 0;
unsigned long lastRTCPoll     = 0;
unsigned long lastOsfCheck    = 0;

const unsigned long COLON_BLINK_MS = 500;
const unsigned long RTC_POLL_MS    = 100;
const unsigned long OSF_CHECK_MS   = 1000;

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

RtcStatus rtcStatus = RtcStatus::Ok;

// ===== Devices =====
Display    display;
SystemTime sysTime(DS3231);
SerialSet  serialSet(115200, sysTime);

void setup() {
  Wire.begin();
  display.begin();
  serialSet.Init();

  // Проверка RTC: связь + OSF-флаг
  if (!sysTime.initCheck()) {
    rtcStatus = RtcStatus::NoComm;
  } else if (!sysTime.isTimeValid()) {
    rtcStatus = RtcStatus::InvalidTime;
  } else {
    rtcStatus = RtcStatus::Ok;
  }

  pollRTC();
  lastRTCPoll     = millis();
  lastColonToggle = millis();
  lastOsfCheck    = millis();

  // Hardware watchdog: 8-секундный таймаут
  ESP.wdtEnable(WDTO_8S);
}

void loop() {
  ESP.wdtFeed();

  unsigned long currentMillis = millis();
  serialSet.SetCMD();

  // 1. Опрос RTC не чаще RTC_POLL_MS
  if (currentMillis - lastRTCPoll >= RTC_POLL_MS) {
    lastRTCPoll = currentMillis;
    pollRTC();
  }

  // 2. Проверка OSF раз в секунду (потеря питания RTC в процессе работы)
  if (currentMillis - lastOsfCheck >= OSF_CHECK_MS) {
    lastOsfCheck = currentMillis;
    sysTime.refreshTimeValid();
  }

  // 3. Мигание двоеточий
  if (currentMillis - lastColonToggle >= COLON_BLINK_MS) {
    lastColonToggle = currentMillis;
    colonVisible = !colonVisible;
    needRedraw = true;
  }

  // 4. Перерисовка только при изменениях
  if (needRedraw) {
    needRedraw = false;
    display.draw(prevHours, prevMinutes, prevSeconds,
                 prevYear, prevMonth, prevDate,
                 colonVisible,
                 prevTemp,
                 prevDay,
                 rtcStatus);
  }
}

void pollRTC() {
  // Проверяем состояние связи и OSF
  RtcStatus newStatus;
  if (!sysTime.getTime()) {
    newStatus = RtcStatus::NoComm;
  } else if (!sysTime.isTimeValid()) {
    newStatus = RtcStatus::InvalidTime;
  } else {
    newStatus = RtcStatus::Ok;
  }

  if (newStatus != rtcStatus) {
    rtcStatus = newStatus;
    needRedraw = true;
  }

  if (rtcStatus == RtcStatus::NoComm) {
    return;
  }

  // Читаем данные (при InvalidTime тоже — температура доступна)
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