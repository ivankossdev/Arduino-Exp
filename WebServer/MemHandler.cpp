#include "MemHandler.h"

Memory::Memory() {
  Serial.begin(115200);
  EEPROM.begin(MEMSIZE);
}

bool Memory::writeString(String str, int memoryPosition) {
  bool write = false;

  if (str.length() <= MEMSIZE) {
    write = true;
    int i = 0;
    do {
      EEPROM.write(memoryPosition, str[i]);
      if (str[i] == '\r' || str[i] == '\n') break;
      i++; memoryPosition++; 
    } while (str[i] != '\0');
    str[i] = '\0';

    commit("Memory EEPROM is write");
  }

  return write;
}

void Memory::writeIntData(int data, int memoryPosition){
  if(memoryPosition < MEMSIZE){
    EEPROM.write(memoryPosition, data);
  }
  commit("Memory EEPROM is write");
}

void Memory::commit(String message){
    if (EEPROM.commit()) {
      Serial.println(message);
    } else {
      Serial.println("eeprom error");
    }
}

void Memory::readString(int memoryPosition) {
  int i = 0;

  do {
    if (i > 512) {
      Serial.println("Error readString()");
      i = 0;
      break;
    }
    buffer[i] = (char)EEPROM.read(memoryPosition);
    i++; memoryPosition++;
  } while (EEPROM.read(i) != '\0');

  buffer[i] = '\0';
}

void Memory::clearAllMemory() {

  for (int i = 0; i < MEMSIZE; i++) {
    EEPROM.write(i, '\0');
    buffer[i] = '\0';
  }

  commit("EEPROM clear");
}

void Memory::clearBuffer(){
  for (int i = 0; i < MEMSIZE; i++) { buffer[i] = '\0'; }
}

Memory memory;

