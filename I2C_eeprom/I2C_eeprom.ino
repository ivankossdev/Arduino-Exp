#include "AT24xx.h"
#define EEPROM_I2C_ADDRESS 0x50

At24 at24(EEPROM_I2C_ADDRESS);
char readArray[32] = { '\0' };

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    String dsCmd = Serial.readString();
    at24.writeWord(0x00, dsCmd.c_str());
    at24.readWord(0x00, readArray);
    Serial.println(readArray);

    // at24.writeByte(0x00, '0');  // Write 0x55 at memory address 0x000A
    // delay(10);                // Small delay for write cycle
    // Serial.println("Write complete.");
    // byte data = at24.readByte(0x00);  // Read the byte from address 0x000A
    // Serial.print("Data read from EEPROM: 0x");
    // Serial.println(data, HEX);
  }
}
