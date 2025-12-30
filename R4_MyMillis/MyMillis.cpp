#include "MyMillis.h"
MyMillis::MyMillis(unsigned long interval_){
  this->interval = interval_; 
}

bool MyMillis::tic_(){
  unsigned long current = millis();
  
  if (current - previous >= interval){
    previous = current;
    state = true; 
  }
  return state;
}
void MyMillis::tic(void (*prnt)(void)){
  if(tic_()){
    prnt(); 
    state = false; 
  }
}

void MyMillis::tic(void (*prnt)(int, int&), int i, int& c){
  if(tic_()){
    prnt(i, c); 
    state = false; 
  }
}

void MyMillis::tic(void(*prnt)(int , String , int l), int row, String message, int lcdColumns){
  if(tic_()){
    prnt(row, message, lcdColumns); 
    state = false;
  }
}









