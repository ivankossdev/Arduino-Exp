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

void SerialSet::CmdHandler(String &data) {
  data.trim();
  data.getBytes(cmd, 4);
  String dataCmd(cmd);
  dataCmd.trim();
  int res = 0;
if (dataCmd.equals("pin")) {
    dataCmd = data.substring(3);

    if (dataCmd.equals(" on")) {
      Serial.println("pin ON");
      digitalWrite(6, HIGH);
      digitalWrite(5, HIGH);
    }
    if (dataCmd.equals(" off")) {
      Serial.println("pin OFF");
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
    }

  } else {
    Serial.println(data + " - cmd is not found");
  }
}


