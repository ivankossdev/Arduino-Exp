#ifndef _MYMILLIS_H
#define _MYMILLIS_H
#include <Arduino.h>

class MyMillis{
  public: 
    MyMillis(unsigned long interval_);
    void tic(void (*prnt)(void));
    void tic(void (*prnt)(int, int&), int i, int& c);
    void tic(void(*prnt)(int , String, int l), int row, String message, int lcdColumns);
  private:
    bool tic_();
    bool state = false;
    long interval; 
    unsigned long previous = 0;
}; 

#endif