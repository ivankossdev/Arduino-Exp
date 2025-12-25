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

/*
  CMD: 
    set2359591   (1 - com set time)
    set12 ... 52 (5 - com set day)
*/

void SerialSet::CmdHandler(String &data) {
  data.trim();
  data.getBytes(cmd, 4);
  String dataCmd(cmd);
  dataCmd.trim();
  int res = 0;

  if (dataCmd.equals("set")) {
    dataCmd = data.substring(3);
    int param = dataCmd.toInt();

    switch (param % 10) {
      case 1: Serial.println("Set time"); break;
      case 2: setDay(conv.TenFormat(param)); break;
      case 3: Serial.println("Set date"); break;
      default: Serial.println("-1");
    }
  } else {
    Serial.println(data + " - cmd is not found");
  }
}

void SerialSet::setDay(int day) {
  Serial.print("Set day ");
  Serial.println(day);

  if (day >= 1 && day <= 7) {
    systime.setDay((uint8_t)day);
  } else {
    Serial.println("Error cmd");
  }
}

uint8_t SerialSet::TwoCharToInt(char c1, char c0) {
  return ((c1 & 0x0f) * 10) + (c0 & 0x0f);
}