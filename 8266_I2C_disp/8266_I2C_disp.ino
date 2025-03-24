#include <cstring>
#include "convertor.h"
#include "display.h"
#include "I2C.h"
#include "ds3231.h"
#include "busset.h"

#define DS3231 0x68
#define MEMORY 0x57

Convertor format;
SystemTime ds(DS3231);
Busset busset;

void setup() {
  displayInit();
  searchDevice();
  ds.SetDay(1);
}

void loop() {
  busset.SetSerial();
  ds.GetTime();
  ds.GetDay();
  showTime();
}

void clearMem(int cnt, char *mem) {
  for (int i = 0; i < cnt; i++)
    mem[i] = '\0';
}

void showTime() {
  sprintf(ds.memDS3231, "Local Time\n%d%d:%d%d:%d%d\nDay %d", format.TenFormat(ds.timeString[2]), format.OneFormat(ds.timeString[2]),
          format.TenFormat(ds.timeString[1]), format.OneFormat(ds.timeString[1]), format.TenFormat(ds.timeString[0]), format.OneFormat(ds.timeString[0]), 
          ds.Day);
  NextRowString(false, ds.memDS3231);
}

