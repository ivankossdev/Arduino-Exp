#include "serialSet.h"

SerialSet::SerialSet() {
  Serial.begin(9600);
  Serial.println("Start programm: ");
}

void SerialSet::SetSerial() {
  String dsCmd;
  if (Serial.available() > 0) {
    dsCmd = Serial.readString();
    Serial.print(dsCmd);
  }
}

uint8_t SerialSet::TwoCharToInt(char c1, char c0) {
  return ((c1 & 0x0f) * 10) + (c0 & 0x0f);
}