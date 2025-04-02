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
bool clearDspEvent = true;

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
  } else if (ds.timeString[1] == 0 && ds.timeString[0] == 0 && clearDspEvent) {
    clearDspEvent = false;
    displayClear();
  } else if (ds.timeString[0] != 0 && !clearDspEvent) {
    clearDspEvent = true;
  }
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
