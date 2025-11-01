#include "ConnectHandler.h"

void waitConnect(bool *state) {
  int timeOutCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    timeOutCount++;
    if (timeOutCount > 20) {
      *state = true;
      break;
    }
  }
}