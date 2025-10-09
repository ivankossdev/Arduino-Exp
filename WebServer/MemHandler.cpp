#include "MemHandler.h"

Memory::Memory() {
  Serial.begin(115200);
  EEPROM.begin(MEMSIZE);
}

bool Memory::writeString(String str) {
  bool write = false;

  if (str.length() <= MEMSIZE) {
    write = true;
    int i = 0;
    do {
      EEPROM.write(i, str[i]);
      if (str[i] == '\r' || str[i] == '\n') break;
      i++;
    } while (str[i] != '\0');
    str[i] = '\0';

    commit("EEPROM write");
  }

  return write;
}

void Memory::commit(String message){
    if (EEPROM.commit()) {
      Serial.println(message);
    } else {
      Serial.println("eeprom error");
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
    buffer[i] = (char)EEPROM.read(i);
    i++;
  } while (EEPROM.read(i) != '\0');

  buffer[i] = '\0';
}

void Memory::clearString() {

  for (int i = 0; i < MEMSIZE; i++) {
    EEPROM.write(i, '\0');
    buffer[i] = '\0';
  }

  commit("EEPROM clear");
}