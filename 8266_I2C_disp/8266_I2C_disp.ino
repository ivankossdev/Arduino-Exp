#include "display.h"
#include <Wire.h>
#include <cstring>

void setup() {
  displayInit();
  Wire.begin();
}

void clearMessage(char *message){
  for(int i = 0; i < 512; i++)
    message[i] = '\0';
}

void loop() {
  char message[512] = {'\0'};
  sprintf(message, "Search I2C:\n");
  NextRowString(false, message);
  delay(2000);
  displayClear();
  int found = 0;

  for(int i = 0; i < 127; i++){
    Wire.beginTransmission(i);

    int result = Wire.endTransmission();

    if(result == 0){
      sprintf(message, "Found 0%x\naddress", i);
      NextRowString(false, message);
      delay(2000);
      clearMessage(message);
      displayClear();
      found++;
    }
  }
  clearMessage(message);
  delay(2000);
}
