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

void setup() {
  displayInit();
  searchDevice();
}

void loop() {
  busset.SetSerial();
  if(busset.isSet){
    busset.isSet = false;
    ds.ClearMemStr(ds.memDS3231, 32);
    displayClear();
  }
  ds.GetTime();
  ds.GetDay();
  ds.GetDate();
  ds.GetMonth();
  ds.GetYear();
  showTime();
}


void showTime() {
  sprintf(ds.memDS3231, "%d%d:%d%d:%d%d\n%s\n%d%d/%d%d/20%d%d", format.TenFormat(ds.timeString[2]), format.OneFormat(ds.timeString[2]),
          format.TenFormat(ds.timeString[1]), format.OneFormat(ds.timeString[1]), format.TenFormat(ds.timeString[0]), format.OneFormat(ds.timeString[0]),
          ds.Day, format.TenFormat(ds.Date), format.OneFormat(ds.Date), format.TenFormat(ds.Month), format.OneFormat(ds.Month), 
          format.TenFormat(ds.Year), format.OneFormat(ds.Year));
  NextRowString(false, ds.memDS3231);
}
