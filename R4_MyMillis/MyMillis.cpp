#include "MyMillis.h"
MyMillis::MyMillis(unsigned long interval_){
  this->interval = interval_; 
}

bool MyMillis::tic(){
  unsigned long current = millis();
  
  if (current - previous >= interval){
    previous = current;
    state = true; 
  }
  return state;
}
bool MyMillis::tic(void (*prnt)(void)){
  unsigned long current = millis();
  
  if (current - previous >= interval){
    prnt();
    previous = current;
    state = true; 
  }
  return state;
}

bool MyMillis::tic(void (*prnt)(int, int&), int i, int& c){
  unsigned long current = millis();
  
  if (current - previous >= interval){
    prnt(i, c);
    previous = current;
    state = true; 
  }
  return state;
}