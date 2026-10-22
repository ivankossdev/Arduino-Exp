#ifndef _DS3231_H
#define _DS3231_H
#include <Arduino.h>
#include "convertor.h"

#define DS3231 0x68

class SystemTime {
public:
  SystemTime(uint8_t address_);

  // Проверка связи + чтение OSF-флага. Вызывать в setup().
  // Возвращает: true — RTC отвечает (связь есть).
  bool initCheck();

  // Периодическая проверка OSF-флага (без чтения времени).
  // Обновляет кэш isTimeValid(). Возвращает true при успешном чтении.
  bool refreshTimeValid();

  // Время: getTime() читает RTC один раз; при ошибке I2C возвращает false.
  bool getTime();
  bool SetTime(int hours, int minutes, int seconds);
  int  getHours()   const { return timeString[2]; }
  int  getMinutes() const { return timeString[1]; }
  int  getSeconds() const { return timeString[0]; }

  // Флаг "время валидно": false = OSF стоял (RTC терял питание).
  bool isTimeValid() const { return timeValidCached; }

  // Дата: каждое чтение — I2C-транзакция. -1 при ошибке.
  int  getDate();
  void setDate(uint8_t data);
  int  getMonth();
  void setMonth(uint8_t data);
  int  getYear();
  void SetYear(uint8_t data);
  const char* getDay();       // "Err" при ошибке
  void setDay(uint8_t data);

  // Температура: при ошибке возвращает предыдущее значение.
  const char* getTemperature();

private:
  uint8_t address;
  Convertor conv;
  int  timeString[3]   = { 0 };
  char temp[7]         = { '0', '0', '.', '0', '0', '\0', '\0' };
  bool timeValidCached = false;

  bool WriteToRegister(uint8_t regAddr, uint8_t data);
  bool ReadRegister(uint8_t regAddr, uint8_t size, uint8_t *data);
  bool clearOscillatorFlag();
};

#endif