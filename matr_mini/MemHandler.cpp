#include "MemHandler.h"

Memory::Memory(){
  Serial.begin(115200);
  EEPROM.begin(512);
  readString();
}

void Memory::writeString() {
  if (Serial.available() > 0) {
    
    clearString();
    String str = Serial.readString();

    if (str.length() <= 512) {

      unsigned int len = str.length();
      Serial.printf("str length = %d\n", len);
      
      int i = 0;
      do {
        EEPROM.write(i, str[i]);
        i++;
        if(str[i] == '\r' || str[i] == '\n') break;
      } while (str[i] != '\0');
      str[i] = '\0';
      
      if (EEPROM.commit()) {
        Serial.println("EEPROM committed");
        readString();
      } else {
        Serial.println("ERROR! EEPROM commit failed");
      }
    }
  }
}

void Memory::readString() {
  int i = 0;
  
  do {
    if (i > 512) {
      Serial.println("Error readString()");
      i = 0;
      break;
    }
    example[i] = (char)EEPROM.read(i);
    i++;
  } while (EEPROM.read(i) != '\0');
  example[i] = '\0';
  Serial.printf("%s\n", example);
}

void Memory::clearString() {
  for (int i = 0; i < 512; i++) { EEPROM.write(i, 0); }
}