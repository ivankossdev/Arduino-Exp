#ifndef _MYMILLIS_H
#define _MYMILLIS_H
#include <Arduino.h>

class MyMillis{
  public: 
    MyMillis(unsigned long interval_);
    bool tic();
    bool tic(void (*prnt)(void));
    bool tic(void (*prnt)(int, int&), int i, int& c);
    bool state = false;
  private:
    long interval; 
    unsigned long previous = 0;
}; 

#endif