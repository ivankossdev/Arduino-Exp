#include "busset.h"
Busset::Busset() {
  Serial.begin(115200);
  Serial.printf("Start programm\n");
}

void Busset::SetSerial() {
  String dsCmd;
  if (Serial.available() > 0) {
    if (!dsCmd.isEmpty())
      dsCmd.clear();
    dsCmd = Serial.readString();
  }
  if (CheckCMD(dsCmd) == SetCmd::time) {
    systime.SetTime(TimeDate::hr, TwoCharToInt(dsCmd[4], dsCmd[5]));
    systime.SetTime(TimeDate::min, TwoCharToInt(dsCmd[6], dsCmd[7]));
    systime.SetTime(TimeDate::sec, TwoCharToInt(dsCmd[8], dsCmd[9]));
  } else if (CheckCMD(dsCmd) == SetCmd::day) {
    systime.SetDay(dsCmd[4] & 0x0f);
  }
}

SetCmd Busset::CheckCMD(String cmd) {
  cmd.trim();
  if (cmd[0] == 's' && cmd[1] == 'e' && cmd[2] == 't' && cmd[3] == 't' && cmd.length() == 10) {
    return SetCmd::time;
  } else if (cmd[0] == 's' && cmd[1] == 'e' && cmd[2] == 't' && cmd[3] == 'd' && cmd.length() == 5) {
    return SetCmd::day;
  }
  return SetCmd::nodata;
}

uint8_t Busset::TwoCharToInt(char c1, char c0) {
  return ((c1 & 0x0f) * 10) + (c0 & 0x0f);
}