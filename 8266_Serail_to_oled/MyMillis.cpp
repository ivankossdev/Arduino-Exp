#include "MyMillis.h"

MyMillis::MyMillis(long interval_){
  this->interval = interval_;
  this->millisState = false;
  this->currentMillis = 0;
  this->previousMillis = 0; 
}

bool MyMillis::millisInterval() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Serial.printf("%d %d %d\n", currentMillis, previousMillis, currentMillis - previousMillis);
    previousMillis = currentMillis;
    if (millisState == false) {
      millisState = true;
    } else {
      millisState = false;
    }
  }

  return millisState;
}