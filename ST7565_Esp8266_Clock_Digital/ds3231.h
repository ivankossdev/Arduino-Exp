#ifndef _DS3231_H
#define _DS3231_H
#include <Arduino.h>
#include "convertor.h"

#define DS3231 0x68

class SystemTime {
public:
  SystemTime(uint8_t address_);

  // Время: getTime() читает RTC один раз, затем getHours/Minutes/Seconds
  // возвращают значения из кэша — без обращения к шине.
  void getTime();
  void SetTime(int hours, int minutes, int seconds);
  int  getHours()   const { return timeString[2]; }
  int  getMinutes() const { return timeString[1]; }
  int  getSeconds() const { return timeString[0]; }

  // Дата: каждое чтение — I2C-транзакция.
  int  getDate();          // день месяца 1..31
  void setDate(uint8_t data);
  int  getMonth();         // месяц 1..12
  void setMonth(uint8_t data);
  int  getYear();          // год 0..99 (после 2000)
  void SetYear(uint8_t data);
  const char* getDay();    // "Mon".."Sun" или "Err"
  void setDay(uint8_t data);

  // Температура: указатель на внутренний буфер (действителен до следующего вызова).
  const char* getTemperature();

private:
  uint8_t address;
  Convertor conv;
  int  timeString[3] = { 0 };
  char temp[7]       = { '0', '0', '.', '0', '0', '\0', '\0' };

  void WriteToRegister(uint8_t regAddr, uint8_t data);
  void ReadRegister(uint8_t regAddr, uint8_t size, uint8_t *data);
};

#endif