#include <Wire.h>
#define EEPROM_I2C_ADDRESS 0x57

void writeEEPROM(unsigned int memAddress, byte data) {
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((memAddress >> 8) & 0xFF);  // High byte of address
  Wire.write(memAddress & 0xFF);         // Low byte of address
  Wire.write(data);                      // Data byte
  Wire.endTransmission();
  delay(5);  
}

byte readEEPROM(unsigned int memAddress) {
  byte data = 0;
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);
  if (Wire.available()) data = Wire.read();
  return data;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  String dsCmd;
  if (Serial.available() > 0) {
    dsCmd = Serial.readString();
    writeEEPROM(0x00, '0');  // Write 0x55 at memory address 0x000A
    delay(10);                // Small delay for write cycle
    Serial.println("Write complete.");
    byte data = readEEPROM(0xf0);  // Read the byte from address 0x000A
    Serial.print("Data read from EEPROM: 0x");
    Serial.println(data, HEX);
  }
}
