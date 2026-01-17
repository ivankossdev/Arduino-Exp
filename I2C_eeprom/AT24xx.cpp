#include "AT24xx.h"

At24::At24(int address_){
  this->address = address_;
  Wire.begin();
}

void At24::writeByte(unsigned int memAddress, byte data) {
  Wire.beginTransmission(address);
  Wire.write((memAddress >> 8) & 0xFF);  // High byte of address
  Wire.write(memAddress & 0xFF);         // Low byte of address
  Wire.write(data);                      // Data byte
  Wire.endTransmission();
  delay(5);  
}

byte At24::readByte(unsigned int memAddress) {
  byte data = 0;
  Wire.beginTransmission(address);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(address, 1);
  if (Wire.available()) data = Wire.read();
  return data;
}

void At24::writeArrayByte(unsigned int memAddress, byte *data, int length){
  Wire.beginTransmission(address);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  for(int i = 0; i < length; i++){
    Wire.write(data[i]);
  }
  Wire.endTransmission();
  delay(5);
}

void At24::readArrayByte(unsigned int memAddress, byte *buffer, int length) {
  Wire.beginTransmission(address);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(address, length);
  for (int i = 0; i < length && Wire.available(); i++) {
    buffer[i] = Wire.read();
  }
}

void At24::writeWord(int memAddress, char *word){
  int i = 0; 
  Wire.beginTransmission(address);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  do{
    Wire.write(word[i]);
    i++;
  }while(word[i] != '\0');
  Wire.write(0);
  Wire.endTransmission();
  delay(5);
}

void At24::readWord(int memAddress, char *word){
  int i = 0; 
  Wire.beginTransmission(address);
  Wire.write((memAddress >> 8) & 0xFF);
  Wire.write(memAddress & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(address, 64);
  do{
    word[i] = Wire.read();
    i++;
  }while(i < Wire.available());
}







