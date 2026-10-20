#include "AT24xx.h"


At24::At24(uint8_t address_){
  this->address = address_;
}

void At24::writeByte(unsigned int memAddress, byte data) {
  Wire.beginTransmission(this->address);
  Wire.write((memAddress >> 8) & 0xFF);  // High byte of address
  Wire.write(memAddress & 0xFF);         // Low byte of address
  Wire.write(data);                      // Data byte
  Wire.endTransmission();
  delay(5);
}

byte At24::readByte(unsigned int memAddress) {
  byte data = 0;
  Wire.beginTransmission(this->address);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)this->address, (uint8_t)1);
  if (Wire.available()) data = Wire.read();
  return data;
}

void At24::writeArrayByte(unsigned int memAddress, byte *data, int length){
  Wire.beginTransmission(this->address);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  for(int i = 0; i < length; i++){
    Wire.write(data[i]);
  }
  Wire.endTransmission();
  delay(5);
}

void At24::readArrayByte(unsigned int memAddress, byte *buffer, int length) {
  Wire.beginTransmission(this->address);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)this->address, (uint8_t)length);
  for (int i = 0; i < length && Wire.available(); i++) {
    buffer[i] = Wire.read();
  }
}

const char* At24::readString(unsigned int memAddress, int length) {
  if (length < 0) length = 0;
  if (length > (int)sizeof(buffer) - 1) length = (int)sizeof(buffer) - 1;

  // Обнуляем диапазон, чтобы при коротком чтении не было мусора
  for (int i = 0; i <= length; i++) {
    buffer[i] = 0;
  }
  readArrayByte(memAddress, buffer, length);
  buffer[length] = '\0';
  return (const char*)buffer;
}