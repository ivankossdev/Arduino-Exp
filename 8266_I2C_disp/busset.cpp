#include "busset.h"
BusSet::BusSet() {
  Serial.begin(115200);
  Serial.printf("Start programm\n");
}

void BusSet::SetSerial() {
  String dsCmd;
  if (Serial.available() > 0) {
    if (!dsCmd.isEmpty())
      dsCmd.clear();
    dsCmd = Serial.readString();
    isSet = true;
  }

  switch (CheckCMD(dsCmd)) {
    case SetCmd::time:
      systime.SetTime(TimeData::hr, TwoCharToInt(dsCmd[4], dsCmd[5]));
      systime.SetTime(TimeData::min, TwoCharToInt(dsCmd[6], dsCmd[7]));
      systime.SetTime(TimeData::sec, TwoCharToInt(dsCmd[8], dsCmd[9]));
      break;
    case SetCmd::day: systime.SetDay(dsCmd[4] & 0x0f); break;
    case SetCmd::date: systime.SetDate(TwoCharToInt(dsCmd[5], dsCmd[6])); break;
  }
  
}

SetCmd BusSet::CheckCMD(String cmd) {
  cmd.trim();
  if (cmd[0] == 's' && cmd[1] == 'e' && cmd[2] == 't' && cmd[3] == 't' && cmd.length() == 10) {
    return SetCmd::time;
  } else if (cmd[0] == 's' && cmd[1] == 'e' && cmd[2] == 't' && cmd[3] == 'd' && cmd.length() == 5) {
    return SetCmd::day;
  } else if (cmd[0] == 's' && cmd[1] == 'e' && cmd[2] == 't' && cmd[3] == 'd' && cmd[4] == 't' && cmd.length() == 7) {
    return SetCmd::date;
  }

  return SetCmd::nodata;
}

uint8_t BusSet::TwoCharToInt(char c1, char c0) {
  return ((c1 & 0x0f) * 10) + (c0 & 0x0f);
}