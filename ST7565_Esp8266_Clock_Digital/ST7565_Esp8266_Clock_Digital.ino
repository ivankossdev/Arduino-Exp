#include <Arduino.h>
#include <Wire.h>
#include "ds3231.h"
#include "serialSet.h"
#include "display.h"

// Текущее состояние времени
int hours   = 0;
int minutes = 0;
int seconds = 0;

// Состояние анимации и таймеры
bool          colonVisible    = true;
unsigned long lastColonToggle = 0;
unsigned long lastRTCPoll     = 0;

const unsigned long COLON_BLINK_MS = 500;   // период мигания двоеточия
const unsigned long RTC_POLL_MS    = 100;   // как часто читаем RTC

// Кеш последних известных значений — чтобы перерисовывать только при изменениях
int  prevHours   = -1;
int  prevMinutes = -1;
int  prevSeconds = -1;
int  prevDate    = -1;
int  prevMonth   = -1;
int  prevYear    = -1;
bool needRedraw  = true;

// Инициализация устройств и обработчика команд
Display    display;
SystemTime sysTime(DS3231);
At24       eeprom(EEPROM_I2C_ADDRESS);
SerialSet  serialSet(115200, sysTime, eeprom);

void setup() {
  Wire.begin();
  display.begin();
  serialSet.Init();

  // Первое чтение RTC — чтобы на экране сразу были реальные значения
  pollRTC();
  lastRTCPoll     = millis();
  lastColonToggle = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  serialSet.SetCMD();

  // 1. Читаем RTC не чаще, чем раз в RTC_POLL_MS
  if (currentMillis - lastRTCPoll >= RTC_POLL_MS) {
    lastRTCPoll = currentMillis;
    pollRTC();
  }

  // 2. Мигание двоеточия
  if (currentMillis - lastColonToggle >= COLON_BLINK_MS) {
    lastColonToggle = currentMillis;
    colonVisible = !colonVisible;
    needRedraw = true;
  }

  // 3. Перерисовываем только при изменениях
  if (needRedraw) {
    needRedraw = false;
    display.draw(hours, minutes, seconds,
                 prevYear, prevMonth, prevDate,
                 colonVisible);
  }
}

void pollRTC() {
  sysTime.getTime();           // одно чтение RTC — кэширует h/m/s
  int y = sysTime.getYear();   // каждое из трёх — отдельная I2C-транзакция
  int m = sysTime.getMonth();
  int d = sysTime.getDate();

  hours   = sysTime.getHours();
  minutes = sysTime.getMinutes();
  seconds = sysTime.getSeconds();

  if (hours != prevHours
      || minutes != prevMinutes
      || seconds != prevSeconds
      || d != prevDate
      || m != prevMonth
      || y != prevYear) {
    prevHours   = hours;
    prevMinutes = minutes;
    prevSeconds = seconds;
    prevDate    = d;
    prevMonth   = m;
    prevYear    = y;
    needRedraw  = true;
  }
}