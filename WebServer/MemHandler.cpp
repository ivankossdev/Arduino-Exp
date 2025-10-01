#include "MemHandler.h"

Memory::Memory(){
  Serial.begin(115200);
  EEPROM.begin(MEMSIZE);
  readString();
}

bool Memory::writeString() {
  bool write = false;

  if (Serial.available() > 0) {
    write = true;
    clearString();
    String str = Serial.readString();

    if (str.length() <= MEMSIZE) {
      int i = 0;
      do {
        EEPROM.write(i, str[i]);
        i++;
        if(str[i] == '\r' || str[i] == '\n') break;
      } while (str[i] != '\0');
      str[i] = '\0';
      
      if (EEPROM.commit()) {
        Serial.println("eeprom write");
        readString();
      } else {
        Serial.println("eeprom error");
      }
    }
  }

  return write;
}

void Memory::readString() {
  int i = 0;
  
  do {
    if (i > 512) {
      Serial.println("Error readString()");
      i = 0;
      break;
    }
    buffer[i] = (char)EEPROM.read(i);
    i++;
  } while (EEPROM.read(i) != '\0');

  buffer[i] = '\0';
  Serial.printf("%s\n", buffer);
}

void Memory::clearString() {
  for (int i = 0; i < 512; i++) { EEPROM.write(i, 0); }
}