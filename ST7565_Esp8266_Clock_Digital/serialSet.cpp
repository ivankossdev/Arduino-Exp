#include "serialSet.h"

namespace {
  const char* CMD_HELP =
    "=== Commands ===\n"
    "help                - this help (or '?')\n"
    "set<HHMMSS>1        - set time (e.g. set2359591)\n"
    "set<YYMMDD>2        - set date (e.g. set2512312)\n"
    "set<D>3             - set weekday 1..7 (e.g. set53 = Fri)\n"
    "get time            - show current time\n"
    "get date            - show current date + weekday\n"
    "mem                 - read first 10 bytes of EEPROM\n"
    "================\n";

  // 2000-2099: високосный = (year % 4 == 0). 2100 в диапазон не входит.
  int daysInMonth(int month, int year99) {
    static const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month < 1 || month > 12) return 0;
    if (month == 2 && (year99 % 4) == 0) return 29;
    return days[month - 1];
  }
}

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
    set13..set73 — установить день недели: set13=Пн, set73=Вс (последняя цифра — 3)
*/

void SerialSet::CmdHandler(String &data) {
  data.trim();

  // "help" длиннее 3 символов — проверяем до getBytes(cmd, 4)
  if (data.equalsIgnoreCase("help") || data.equals("?")) {
    Serial.print(CMD_HELP);
    return;
  }

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
    Serial.println(data + " - cmd is not found (try 'help')");
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
  if (day < 1 || day > 7) {
    Serial.println("Error: weekday must be 1..7");
    return;
  }
  systime.setDay((uint8_t)day);
  Serial.print("Weekday set to ");
  Serial.println(day);
}

void SerialSet::setTime(long time) {
  int hms[3] = { 0 };
  formatData(time, hms);
  // hms[0]=sec, hms[1]=min, hms[2]=hr
  int sec = hms[0];
  int min = hms[1];
  int hr  = hms[2];

  if (hr < 0 || hr > 23 || min < 0 || min > 59 || sec < 0 || sec > 59) {
    Serial.println("Error: time out of range (HH=0..23, MM=0..59, SS=0..59)");
    return;
  }

  systime.SetTime(hr, min, sec);
  Serial.print("Time set to ");
  Serial.print(hr);  Serial.print(":");
  Serial.print(min); Serial.print(":");
  Serial.println(sec);
}

void SerialSet::setDate(long date) {
  int ymd[3] = { 0 };
  formatData(date, ymd);
  // ymd[0]=day, ymd[1]=month, ymd[2]=year
  int day   = ymd[0];
  int month = ymd[1];
  int year  = ymd[2];

  if (year < 0 || year > 99) {
    Serial.println("Error: year must be 0..99");
    return;
  }
  if (month < 1 || month > 12) {
    Serial.println("Error: month must be 1..12");
    return;
  }
  int dim = daysInMonth(month, year);
  if (day < 1 || day > dim) {
    Serial.print("Error: day must be 1..");
    Serial.print(dim);
    Serial.print(" for month ");
    Serial.println(month);
    return;
  }

  systime.setDate(day);
  systime.setMonth(month);
  systime.SetYear(year);
  Serial.print("Date set to ");
  Serial.print(2000 + year); Serial.print("-");
  Serial.print(month);       Serial.print("-");
  Serial.println(day);
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