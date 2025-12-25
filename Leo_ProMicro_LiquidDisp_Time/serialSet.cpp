#include "serialSet.h"


SerialSet::SerialSet(unsigned long speed) {
  Serial.begin(speed);
}

void SerialSet::SetCMD() {
  String dsCmd;
  if (Serial.available() > 0) {
    dsCmd = Serial.readString();
    CmdHandler(dsCmd);
  }
}
void SerialSet::CmdHandler(String &data){
    Serial.println(data);
    data.getBytes(cmd, 4);
    Serial.println(cmd);
    Serial.println(data.substring(4));
}

uint8_t SerialSet::TwoCharToInt(char c1, char c0) {
  return ((c1 & 0x0f) * 10) + (c0 & 0x0f);
}