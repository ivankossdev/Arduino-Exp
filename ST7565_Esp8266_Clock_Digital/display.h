#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

// Состояние RTC, влияющее на отрисовку
enum class RtcStatus {
  Ok,           // связь есть, время валидно
  NoComm,       // нет связи по I2C
  InvalidTime   // связь есть, но OSF стоял (время не установлено)
};

class Display {
public:
  Display();
  void begin();

  void draw(int hours, int minutes, int seconds,
            int year, int month, int date,
            bool colonVisible,
            const char* temperature,
            const char* weekday,
            RtcStatus rtcStatus);

private:
  U8G2_ST7565_ERC12864_F_4W_HW_SPI u8g2;

  void drawTime(int h, int m, int s, bool colonVisible);
  void drawTemperature(const char* temp);
  void drawDate(int year, int month, int date, const char* weekday);
  void drawRtcNoComm();
  void drawRtcInvalidTime(const char* temperature);
};

#endif