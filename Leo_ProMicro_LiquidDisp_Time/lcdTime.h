#ifndef _LCDTIME_H_
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "convertor.h"

class LcdTime : public LiquidCrystal_I2C {
public:
  using LiquidCrystal_I2C::LiquidCrystal_I2C;
  void printTime(int h, int m, int s); 
  void printDate(int year, int date, int month);
  
private:
  Convertor conv; 
};

#endif