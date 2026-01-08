#include "AT24xx.h"


At24::At24(uint8_t address_){
  this->address = address_;
  Wire.begin();
}

void At24::writeByte(unsigned int memAddress, byte data) {
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((memAddress >> 8) & 0xFF);  // High byte of address
  Wire.write(memAddress & 0xFF);         // Low byte of address
  Wire.write(data);                      // Data byte
  Wire.endTransmission();
  delay(5);  
}

byte At24::readByte(unsigned int memAddress) {
  byte data = 0;
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);
  if (Wire.available()) data = Wire.read();
  return data;
}

void At24::writeArrayByte(unsigned int memAddress, byte *data, int length){
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  for(int i = 0; i < length; i++){
    Wire.write(data[i]);
  }
  Wire.endTransmission();
  delay(5);
}

void At24::readArrayByte(unsigned int memAddress, byte *buffer, int length) {
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_I2C_ADDRESS, length);
  for (int i = 0; i < length && Wire.available(); i++) {
    buffer[i] = Wire.read();
  }
}






