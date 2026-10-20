#include "serialSet.h"

SerialSet::SerialSet(unsigned long speed_, SystemTime& systime_, At24& at24_)
  : systime(systime_), at24(at24_) {
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
    set2359591   — установить время 23:59:59 (последняя цифра — 1)
    set2512312   — установить дату 25-12-31  (последняя цифра — 2)
    set13..set53 — установить день недели: set13=Пн, set53=Пт (последняя цифра — 3)
*/

void SerialSet::CmdHandler(String &data) {
  data.trim();

  // Локальный буфер: 3 символа команды + '\0'
  char cmd[4] = { '\0' };
  data.getBytes((unsigned char *)cmd, 4);

  String dataCmd(cmd);
  dataCmd.trim();

  if (dataCmd.equals("set")) {
    dataCmd = data.substring(3);
    long param = dataCmd.toInt();

    switch (param % 10) {
      case 1: setTime(param); break;
      case 2: setDate(param); break;
      case 3: setDay(conv.tensDigit(param)); break;
      default: Serial.println("-1");
    }
  } else if (dataCmd.equals("get")) {
    dataCmd = data.substring(3);

    if (dataCmd.equals(" time")) {
      showTime();
    }
    if (dataCmd.equals(" date")) {
      showDate();
    }

  } else if (dataCmd.equals("mem")) {
    Serial.println("Read memory address 0x00 + 10: ");
    Serial.println(at24.readString(0x00, 10));

  } else {
    Serial.println(data + " - cmd is not found");
  }
}

void SerialSet::showTime() {
  systime.getTime();
  Serial.print("Time: ");
  Serial.print(systime.getHours());
  Serial.print(":");
  Serial.print(systime.getMinutes());
  Serial.print(":");
  Serial.println(systime.getSeconds());
}

void SerialSet::showDate() {
  int year  = systime.getYear();
  int month = systime.getMonth();
  int date  = systime.getDate();
  const char* day = systime.getDay();

  Serial.print("Date: ");
  Serial.print(year);
  Serial.print("-");
  Serial.print(month);
  Serial.print("-");
  Serial.print(date);
  Serial.print(" ");
  Serial.println(day);
}

void SerialSet::setDay(int day) {
  if (day >= 1 && day <= 7) {
    systime.setDay((uint8_t)day);
  } else {
    Serial.println("Error cmd");
  }
}

void SerialSet::setTime(long time) {
  int hms[3] = { 0 };
  formatData(time, hms);
  // hms[0]=sec, hms[1]=min, hms[2]=hr
  systime.SetTime(hms[2], hms[1], hms[0]);
}

void SerialSet::setDate(long date) {
  int ymd[3] = { 0 };
  formatData(date, ymd);
  // ymd[0]=day, ymd[1]=month, ymd[2]=year
  systime.setDate(ymd[0]);
  systime.setMonth(ymd[1]);
  systime.SetYear(ymd[2]);
}

// Раскладывает `data` (без последней цифры-команды) на три двузначных поля
// [0] = младший байт (sec/day), [1] = средний (min/month), [2] = старший (hr/year)
void SerialSet::formatData(long data, int *intData) {
  long temp = data / 10;
  for (int i = 0; i < 3; i++) {
    intData[i] = int(temp % 100);
    temp = temp / 100;
  }
}