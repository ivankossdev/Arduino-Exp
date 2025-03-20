#include "I2C.h"

void clearMessage(char *message){
  for(int i = 0; i < LEN_MESSAGE; i++)
    message[i] = '\0';
}

void searchDevice(){
  char message[LEN_MESSAGE] = {'\0'};
  sprintf(message, "Search I2C:\n");
  NextRowString(false, message);
  delay(2000);
  displayClear();
  int found = 0;

  for(int i = 0; i < 127; i++){
    Wire.beginTransmission(i);

    int result = Wire.endTransmission();

    if(result == 0){
      sprintf(message, "Found 0x%x\naddress", i);
      NextRowString(false, message);
      delay(2000);
      clearMessage(message);
      displayClear();
      found++;
    }
  }
  clearMessage(message);
  delay(500);
}