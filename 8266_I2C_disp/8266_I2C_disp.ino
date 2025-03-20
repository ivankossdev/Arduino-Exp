#include "display.h"
#include "I2C.h"
#include <cstring>
#include "convertor.h"


#define DS3231 0x68
#define MEMORY 0x57
#define DELAY 2000

char memDS3231[32] = { '\0' };
char format[32] = { '\0' };
Convertor conv;

void clearMem(int cnt, char *mem) {
  for (int i = 0; i < cnt; i++)
    mem[i] = '\0';
}

void setup() {
  displayInit();
  Wire.begin();
  searchDevice();
}

void loop() {
  int c = 0, c0 = 0, c1 = 0, res = 0;
  Wire.beginTransmission(DS3231);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)DS3231, (size_t)3, true);

  while (Wire.available()) {
    res = conv.FromEightToDec(Wire.read());
    c1 = res / 10 % 10;
    c0 = res % 10;
    sprintf(format, "[%dh] %d%d\n", c, c1, c0);
    strcat(memDS3231, format);
    NextRowString(false, memDS3231);
    c++;
  }
  clearMem(32, memDS3231);
}
