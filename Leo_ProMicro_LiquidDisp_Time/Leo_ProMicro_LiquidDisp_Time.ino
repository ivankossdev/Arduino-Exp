#include <Wire.h>
#include "lcdTime.h"
#include "I2C_Device.h"
#include "ds3231.h"
#include "serialSet.h"
#define printByte(args) write(args);

// Leonardo pro micro
// 2 SDA
// 3 SCL

int lcdColumns = 16;
int lcdRows = 2;

LcdTime lcdTime(0x27, lcdColumns, lcdRows);
I2C_Device i2cDev; 
SystemTime ds(DS3231);
SerialSet set(9600);


void setup() {
  lcdTime.init();
  lcdTime.backlight();
  lcdTime.clear();
  delay(100);
  Serial.begin(9600);
  Serial.println(i2cDev.searchDevice());
}

void loop() {
  ds.getTime();
  lcdTime.printTime(ds.timeString[2], ds.timeString[1], ds.timeString[0]);
  ds.getDate();
  ds.getMonth();
  ds.getYear(); 
  ds.getDay();

  lcdTime.printDate(ds.Year, ds.Date, ds.Month, ds.Day);
  set.SetSerial();
}
