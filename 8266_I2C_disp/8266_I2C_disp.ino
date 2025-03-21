#include "display.h"
#include "I2C.h"
#include <cstring>
#include "convertor.h"


#define DS3231 0x68
#define MEMORY 0x57

char memDS3231[32] = { '\0' };
int timeString[3] = { '\0' };
Convertor conv;


void setup() {
  displayInit();
  Wire.begin();
  searchDevice();
}

void loop() {
  getTime();
  showTime();
}

void clearMem(int cnt, char *mem) {
  for (int i = 0; i < cnt; i++)
    mem[i] = '\0';
}

void getTime() {
  int c = 0;
  Wire.beginTransmission(DS3231);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)DS3231, (size_t)3, true);

  while (Wire.available()) {
    timeString[c] = conv.FromEightToDec(Wire.read());
    c++;
  }
}

void showTime() {
  sprintf(memDS3231, "Local Time\n%d%d:%d%d:%d%d", conv.TenFormat(timeString[2]), conv.OneFormat(timeString[2]),
          conv.TenFormat(timeString[1]), conv.OneFormat(timeString[1]), conv.TenFormat(timeString[0]), conv.OneFormat(timeString[0]));
  NextRowString(false, memDS3231);
  clearMem(32, memDS3231);
}