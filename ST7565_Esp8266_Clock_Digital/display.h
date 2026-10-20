#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

class Display {
public:
  Display();

  // Инициализация U8G2: begin, контраст, режим шрифтов
  void begin();

  // Полная перерисовка экрана.
  // year — 0..99 (после 2000, как хранит DS3231);
  // colonVisible — рисовать ли двоеточие в текущем кадре.
  void draw(int hours, int minutes, int seconds,
            int year, int month, int date,
            bool colonVisible);

private:
  U8G2_ST7565_ERC12864_F_4W_HW_SPI u8g2;

  void drawMainTime(int hours, int minutes, bool colonVisible);
  void drawSecondaryInfo(int seconds, int year, int month, int date);
  void drawDecorations();
};

#endif