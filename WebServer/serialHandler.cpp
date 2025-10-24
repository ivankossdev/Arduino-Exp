#include "serialHandler.h"


void SerialHandler::serialReader() {
  if (Serial.available() > 0) {
    serialData = Serial.readString();
    serialData.trim();
    int i = 0;
    do {
      if (serialData[i] == '\r' || serialData[i] == '\n') break;
      Serial.printf("%c", serialData[i]);
      i++;
    } while (serialData[i] != '\0');
    
    Serial.printf("\n");
  }
}