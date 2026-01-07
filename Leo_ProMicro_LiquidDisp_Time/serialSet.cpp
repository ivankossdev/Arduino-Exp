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

  if (dataCmd.equals("set")) {
    dataCmd = data.substring(3);
    long param = dataCmd.toInt();

    switch (param % 10) {
      case 1: setTime(param); break;
      case 2: setDate(param); break;
      case 3: setDay(conv.TenFormat(param)); break;
      default: Serial.println("-1");
    }
  } else if (dataCmd.equals("i2c")) {
    Serial.println(i2cDev.searchDevice());
  } else if (dataCmd.equals("mem")) {
    Serial.println("test memory: ");
    
  } else {
    Serial.println(data + " - cmd is not found");
  }
}

void SerialSet::setDay(int day) {
  if (day >= 1 && day <= 7) {
    systime.setDay((uint8_t)day);
  } else {
    Serial.println("Error cmd");
  }
}

void SerialSet::setTime(long time) {
  formatData(time, insertData);
  systime.SetTime(insertData);
}

void SerialSet::setDate(long date) {
  formatData(date, insertData);
  systime.setDate(insertData[0]);
  systime.setMonth(insertData[1]);
  systime.SetYear(insertData[2]);
}


void SerialSet::formatData(long data, int *intData) {
  long temp = data / 10;
  for (int i = 0; i < 3; i++) {
    insertData[i] = int(temp % 100);
    temp = temp / 100;
  }
}
