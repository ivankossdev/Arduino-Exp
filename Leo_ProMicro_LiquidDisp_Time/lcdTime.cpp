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

void LcdTime::printDate(int year, int date, int month, String Day) {
  setCursor(0, 1);
  print("20");

  setCursor(2, 1);
  print(conv.TenFormat(year));
  setCursor(3, 1);
  print(conv.OneFormat(year));

  setCursor(4, 1);
  print('-');

  setCursor(5, 1);
  print(conv.TenFormat(date));
  setCursor(6, 1);
  print(conv.OneFormat(date));

  setCursor(7, 1);
  print('-');

  setCursor(8, 1);
  print(conv.TenFormat(month));
  setCursor(9, 1);
  print(conv.OneFormat(month));

  setCursor(9, 0);
  
  print(Day);
}
