#include <Wire.h>
#include "lcdTime.h"
#include "I2C_Device.h"
#include "ds3231.h"

// Leonardo pro micro
// 2 SDA
// 3 SCL

#define printByte(args) write(args);
int lcdColumns = 16;
int lcdRows = 2;

LcdTime lcdTime(0x27, lcdColumns, lcdRows);
I2C_Device i2cDev; 
SystemTime ds(DS3231);

void setup() {
  lcdTime.init();
  lcdTime.backlight();
  lcdTime.clear();
  Serial.begin(9600);
  Serial.println(i2cDev.searchDevice());
}

void loop() {
  ds.getTime();
  lcdTime.printTime(ds.timeString[2], ds.timeString[1], ds.timeString[0]);
  ds.getDate();
  lcdTime.printDate(ds.Date);
}
