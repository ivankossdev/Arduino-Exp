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
      systime.SetTime(TimeData::hr, TwoCharToInt(dsCmd[5], dsCmd[6]));
      systime.SetTime(TimeData::min, TwoCharToInt(dsCmd[7], dsCmd[8]));
      systime.SetTime(TimeData::sec, TwoCharToInt(dsCmd[9], dsCmd[10]));
      break;
    case SetCmd::day: systime.SetDay(dsCmd[5] & 0x0f); break;
    case SetCmd::date: systime.SetDate(TwoCharToInt(dsCmd[5], dsCmd[6])); break;
    case SetCmd::month: systime.SetMonth(TwoCharToInt(dsCmd[5], dsCmd[6])); break;
  }
}

SetCmd BusSet::CheckCMD(String cmd) {
  cmd.trim();
  if (cmd[0] == 's' && cmd[1] == 'e' && cmd[2] == 't' && cmd[3] == 't' && cmd[4] == 'm' && cmd.length() == 11) {
    return SetCmd::time;
  } else if (cmd[0] == 's' && cmd[1] == 'e' && cmd[2] == 't' && cmd[3] == 'd' && cmd[4] == 'y' && cmd.length() == 6) {
    return SetCmd::day;
  } else if (cmd[0] == 's' && cmd[1] == 'e' && cmd[2] == 't' && cmd[3] == 'd' && cmd[4] == 't' && cmd.length() == 7) {
    return SetCmd::date;
  } else if (cmd[0] == 's' && cmd[1] == 'e' && cmd[2] == 't' && cmd[3] == 'm' && cmd[4] == 'n' && cmd.length() == 7) {
    return SetCmd::month;
  }

  return SetCmd::nodata;
}

uint8_t BusSet::TwoCharToInt(char c1, char c0) {
  return ((c1 & 0x0f) * 10) + (c0 & 0x0f);
}