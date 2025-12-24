#ifndef _LCDTIME_H_
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class LcdTime : public LiquidCrystal_I2C {
public:
  using LiquidCrystal_I2C::LiquidCrystal_I2C;
  void printTime(int h, int m, int s); 
  void printDate(int date);
private:
  int S0 = 0;
  int S1 = 0;
  int M0 = 0;
  int M1 = 0;
  int H0 = 0;
  int H1 = 0;
  int Date0 = 0;
  int Date1 = 0;
};

#endif