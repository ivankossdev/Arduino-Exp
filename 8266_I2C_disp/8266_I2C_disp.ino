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
BusSet busset;
String chekDay = "";

void setup() {
  displayInit();
  searchDevice();
}

void loop() {
  displayClearToEvent();
  ds.GetTime();
  ds.GetDay();
  ds.GetDate();
  ds.GetMonth();
  ds.GetYear();
  ds.GetTemperature();
  showTime();
}

void displayClearToEvent() {
  busset.SetSerial();
  if (busset.isSet) {
    busset.isSet = false;
    displayClear();
    ds.ClearMemStr(ds.memDS3231, 32);
  }
  if (chekDay != ds.Day) {
    displayClear();
    ds.ClearMemStr(ds.memDS3231, 32);
  }

  chekDay = ds.Day;
}

void showTime() {
  sprintf(ds.memDS3231, "%d%d/%d%d/20%d%d\n%s\n%d%d:%d%d:%d%d\nT%sC",
          format.TenFormat(ds.Date), format.OneFormat(ds.Date),
          format.TenFormat(ds.Month), format.OneFormat(ds.Month),
          format.TenFormat(ds.Year), format.OneFormat(ds.Year),
          ds.Day,
          format.TenFormat(ds.timeString[2]), format.OneFormat(ds.timeString[2]),
          format.TenFormat(ds.timeString[1]), format.OneFormat(ds.timeString[1]),
          format.TenFormat(ds.timeString[0]), format.OneFormat(ds.timeString[0]),
          ds.Temp);
  NextRowString(false, ds.memDS3231);
}
