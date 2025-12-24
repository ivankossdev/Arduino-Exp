#include "lcdTime.h"
void LcdTime::printTime(int h, int m, int s) {
  S0 = s / 10;
  S1 = s % 10;
  M0 = m / 10;
  M1 = m % 10;
  H0 = h / 10;
  H1 = h % 10;

  setCursor(0, 0);
  print(H0);
  setCursor(1, 0);
  print(H1);

  setCursor(2, 0);
  print(':');

  setCursor(3, 0);
  print(M0);
  setCursor(4, 0);
  print(M1);

  setCursor(5, 0);
  print(':');

  setCursor(6, 0);
  print(S0);
  setCursor(7, 0);
  print(S1);
}

void LcdTime::printDate(int date) {
  Date0 = date / 10;
  Date1 = date % 10;
  setCursor(2, 1);
  print('-');

  setCursor(3, 1);
  print(Date0);
  setCursor(4, 1);
  print(Date1);

  setCursor(5, 1);
  print('-');
}
