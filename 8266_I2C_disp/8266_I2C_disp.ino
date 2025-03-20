#include "display.h"
#include "I2C.h"
#include <cstring>


#define DS3231 0x68
#define MEMORY 0x57
#define DELAY 2000

char memDS3231[32] = {'\0'};

void clearMem(int cnt, char *mem){
  for(int i = 0; i < cnt; i++)
    mem[i] = '\0';
}

void setup() {
  displayInit();
  Wire.begin();
  searchDevice();
}

void loop() {
  NextRowString(false, (char *)"Read byte\nby addr 0x68");
  delay(DELAY);
  displayClear();
  Wire.requestFrom(DS3231, 2);
  while (Wire.available()){
    sprintf(memDS3231, "tx=0x%x", Wire.read());
    NextRowString(false, memDS3231);
    delay(DELAY);
  }
  clearMem(32, memDS3231);
  displayClear();
}





