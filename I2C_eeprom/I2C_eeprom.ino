#include "AT24xx.h"
#define EEPROM_I2C_ADDRESS 0x50

At24 at24(EEPROM_I2C_ADDRESS);
char readArray[LENGHT_PAGE] = { '\0' };

void setup() {
  Serial.begin(9600);
  at24.readWord(0x00, readArray);
  Serial.println(readArray);
}

void loop() {
  if (Serial.available() > 0) {
    at24.clearPage(0x00);
    String dsCmd = Serial.readString();
    at24.writeWord(0x00, dsCmd.c_str());
    at24.readWord(0x00, readArray);
    Serial.println(readArray);
  }
}
