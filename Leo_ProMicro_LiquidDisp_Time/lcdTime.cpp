#include "lcdTime.h"
void LcdTime::printTime(int h, int m, int s) {
  setCursor(0, 0);
  print(conv.TenFormat(h));
  setCursor(1, 0);
  print(conv.OneFormat(h));

  setCursor(2, 0);
  print(':');

  setCursor(3, 0);
  print(conv.TenFormat(m));
  setCursor(4, 0);
  print(conv.OneFormat(m));

  setCursor(5, 0);
  print(':');

  setCursor(6, 0);
  print(conv.TenFormat(s));
  setCursor(7, 0);
  print(conv.OneFormat(s));
}

void LcdTime::printDate(int date, int month) {

  setCursor(2, 1);
  print('-');

  setCursor(3, 1);
  print(conv.TenFormat(date));
  setCursor(4, 1);
  print(conv.OneFormat(date));

  setCursor(5, 1);
  print('-');

  setCursor(6, 1);
  print(conv.TenFormat(month));
  setCursor(7, 1);
  print(conv.OneFormat(month));
}
