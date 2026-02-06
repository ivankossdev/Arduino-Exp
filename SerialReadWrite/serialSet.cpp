#include "serialSet.h"

SerialSet::SerialSet(unsigned long speed_) {
  this->speed = speed_;
}

void SerialSet::Init() {
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
    set2359591   (1 - com set time 23:59:59)
    set2512312.  (2 - com set date 25-12-31)
    set13 ... 53 (3 - com set 5 day fri)
*/

void SerialSet::CmdHandler(String &data) {
  data.trim();
  data.getBytes(cmd, 4);
  String dataCmd(cmd);
  dataCmd.trim();
  int res = 0;
if (dataCmd.equals("get")) {
    dataCmd = data.substring(3);

    if (dataCmd.equals(" time")) {
      Serial.println("CMD get time");
    }
    if (dataCmd.equals(" date")) {
      Serial.println("CMD get date");
    }

  } else {
    Serial.println(data + " - cmd is not found");
  }
}


