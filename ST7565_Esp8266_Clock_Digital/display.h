#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

class Display {
public:
  Display();

  // Инициализация U8G2
  void begin();

  // Полная перерисовка. year — 0..99 (после 2000, как хранит DS3231);
  // temperature — строка вида "25.00" или "-5.25";
  // colonVisible — рисовать ли оба двоеточия в текущем кадре.
  void draw(int hours, int minutes, int seconds,
            int year, int month, int date,
            bool colonVisible,
            const char* temperature);

private:
  U8G2_ST7565_ERC12864_F_4W_HW_SPI u8g2;

  void drawTime(int h, int m, int s, bool colonVisible);
  void drawTemperature(const char* temp);
  void drawDate(int year, int month, int date);
};

#endif