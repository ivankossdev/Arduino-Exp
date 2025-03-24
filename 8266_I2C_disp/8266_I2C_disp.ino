#include <cstring>
#include "convertor.h"
#include "display.h"
#include "I2C.h"
#include "ds3231.h"

#define DS3231 0x68
#define MEMORY 0x57

Convertor format;
SysClock ds(DS3231);

void setup() {
  displayInit();
  searchDevice();
  Serial.begin(115200);
}

void loop() {
  setSerial();
  ds.GetTime();
  showTime();
}

void clearMem(int cnt, char *mem) {
  for (int i = 0; i < cnt; i++)
    mem[i] = '\0';
}

void showTime() {
  sprintf(ds.memDS3231, "Local Time\n%d%d:%d%d:%d%d", format.TenFormat(ds.timeString[2]), format.OneFormat(ds.timeString[2]),
          format.TenFormat(ds.timeString[1]), format.OneFormat(ds.timeString[1]), format.TenFormat(ds.timeString[0]), format.OneFormat(ds.timeString[0]));
  NextRowString(false, ds.memDS3231);
}

void setSerial() {
  String dsCmd;
  if (Serial.available() > 0) {
    if (!dsCmd.isEmpty())
      dsCmd.clear();
    dsCmd = Serial.readString();
  }
  if (checkCMD(dsCmd)) {
    Serial.printf("%s", dsCmd);
  }
}

bool checkCMD(String cmd) {
  if (cmd[0] == 's' && cmd[1] == 'e' && cmd[2] == 't')
    return true;
  else
    return false;
}
