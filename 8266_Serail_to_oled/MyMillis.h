#ifndef _MYMILLIS_H
#define _MYMILLIS_H
#include <Arduino.h>

class MyMillis {
public:
  MyMillis(long interval_);
  bool millisInterval(); 
private:
  unsigned long previousMillis;
  unsigned long currentMillis;
  bool millisState;
  long interval; 
};

#endif