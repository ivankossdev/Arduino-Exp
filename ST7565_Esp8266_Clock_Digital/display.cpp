#include "display.h"

namespace {
  // DS3231 хранит только 2 цифры года; считаем, что век — 2000-е
  constexpr int YEAR_BASE = 2000;
}

Display::Display()
  : u8g2(U8G2_R0, /* cs=*/D8, /* dc=*/D4, /* reset=*/ 0) {
}

void Display::begin() {
  u8g2.begin();
  u8g2.setContrast(5);   // Оптимально для ST7565: 5 - 15
  u8g2.setFontMode(0);   // Режим с заливкой фона
  u8g2.setFontDirection(0);
}

void Display::draw(int hours, int minutes, int seconds,
                   int year, int month, int date,
                   bool colonVisible) {
  u8g2.firstPage();
  do {
    // Очищаем дисплей
    u8g2.setDrawColor(0);
    u8g2.drawBox(0, 0, 128, 64);
    u8g2.setDrawColor(1);

    drawMainTime(hours, minutes, colonVisible);
    drawSecondaryInfo(seconds, year, month, date);
    drawDecorations();
  } while (u8g2.nextPage());
}

void Display::drawMainTime(int hours, int minutes, bool colonVisible) {
  char hourStr[3], minuteStr[3];

  // Форматирование с ведущим нулем
  sprintf(hourStr, "%02d", hours);
  sprintf(minuteStr, "%02d", minutes);

  u8g2.setFont(u8g2_font_logisoso20_tn);

  // Рассчитываем позиции
  int colonWidth = 10;                      // Ширина двоеточия
  int digitWidth = u8g2.getStrWidth("88");  // Ширина двух цифр
  int totalWidth = digitWidth * 2 + colonWidth;
  int startX = (128 - totalWidth) / 2;
  int yPos = 23;                            // Отступ сверху

  // Часы
  u8g2.drawStr(startX, yPos, hourStr);

  // Двоеточие (мигающее)
  if (colonVisible) {
    u8g2.setFont(u8g2_font_logisoso20_tn);
    u8g2.drawStr(startX + digitWidth, yPos, ":");
  }

  // Минуты
  u8g2.drawStr(startX + digitWidth + colonWidth, yPos, minuteStr);
}

void Display::drawSecondaryInfo(int seconds, int year, int month, int date) {
  char secondStr[3] = { '\0' };
  char dateStr[16]  = { '\0' };

  sprintf(secondStr, "%02d", seconds);
  // YEAR_BASE + year даёт полный 4-значный год ("2025", "2005", ...)
  sprintf(dateStr, "%04d.%02d.%02d", YEAR_BASE + year, month, date);

  // Секунды (под основным временем)
  u8g2.setFont(u8g2_font_logisoso16_tf);    // 16px высотой
  int secWidth = u8g2.getStrWidth(secondStr);
  int secX = (128 - secWidth) / 2;
  u8g2.drawStr(secX, 40, secondStr);

  // Дата
  u8g2.setFont(u8g2_font_7x13B_tf);         // 13px высотой
  u8g2.drawStr(30, 55, dateStr);
}

void Display::drawDecorations() {
  // Верхняя разделительная линия
  u8g2.drawHLine(10, 41, 108);

  // Угловые элементы
  u8g2.drawFrame(0, 0, 128, 64);
  u8g2.drawPixel(0, 0);
  u8g2.drawPixel(127, 0);
  u8g2.drawPixel(0, 63);
  u8g2.drawPixel(127, 63);
}