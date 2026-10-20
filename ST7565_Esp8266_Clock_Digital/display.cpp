#include "display.h"

namespace {
  constexpr int YEAR_BASE = 2000;
}

Display::Display()
  : u8g2(U8G2_R0, /* cs=*/D8, /* dc=*/D4, /* reset=*/ 0) {
}

void Display::begin() {
  u8g2.begin();
  u8g2.setContrast(5);
  u8g2.setFontMode(0);
  u8g2.setFontDirection(0);
}

void Display::draw(int hours, int minutes, int seconds,
                   int year, int month, int date,
                   bool colonVisible,
                   const char* temperature,
                   const char* weekday) {
  u8g2.firstPage();
  do {
    // Очистка
    u8g2.setDrawColor(0);
    u8g2.drawBox(0, 0, 128, 64);
    u8g2.setDrawColor(1);

    // Рамка
    u8g2.drawFrame(0, 0, 128, 64);

    // Разделители строк
    u8g2.drawHLine(2, 21, 124);
    u8g2.drawHLine(2, 42, 124);

    drawTime(hours, minutes, seconds, colonVisible);
    drawTemperature(temperature);
    drawDate(year, month, date, weekday);
  } while (u8g2.nextPage());
}

void Display::drawTime(int h, int m, int s, bool colonVisible) {
  char hh[3], mm[3], ss[3];
  sprintf(hh, "%02d", h);
  sprintf(mm, "%02d", m);
  sprintf(ss, "%02d", s);

  u8g2.setFont(u8g2_font_logisoso16_tn);
  int digitW = u8g2.getStrWidth("00");
  int colonW = u8g2.getStrWidth(":");
  int totalW = digitW * 3 + colonW * 2;
  int x = (128 - totalW) / 2;
  int y = 19;  // baseline в строке y=1..20

  u8g2.drawStr(x, y, hh);
  if (colonVisible) u8g2.drawStr(x + digitW, y, ":");
  u8g2.drawStr(x + digitW + colonW, y, mm);
  if (colonVisible) u8g2.drawStr(x + digitW * 2 + colonW, y, ":");
  u8g2.drawStr(x + digitW * 2 + colonW * 2, y, ss);
}

void Display::drawTemperature(const char* temp) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%s\xB0" "C", temp);

  u8g2.setFont(u8g2_font_7x13B_tf);
  int w = u8g2.getStrWidth(buf);
  u8g2.drawStr((128 - w) / 2, 37, buf);
}

void Display::drawDate(int year, int month, int date, const char* weekday) {
  char buf[24];
  // "YYYY.MM.DD  Mon" — дата и день недели через два пробела
  snprintf(buf, sizeof(buf), "%04d.%02d.%02d  %s",
           YEAR_BASE + year, month, date, weekday);

  u8g2.setFont(u8g2_font_7x13B_tf);
  int w = u8g2.getStrWidth(buf);
  u8g2.drawStr((128 - w) / 2, 58, buf);
}