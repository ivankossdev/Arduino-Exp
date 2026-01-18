#ifndef _AT24XX_H_
#define _AT24XX_H_
#include <Arduino.h>
#include <Wire.h>

#define LENGHT_PAGE 64

class At24{
public:
  At24(int address_);
  void writeByte(unsigned int memAddress, byte data);
  byte readByte(unsigned int memAddress);
  void writeArrayByte(unsigned int memAddress, byte *data, int length);
  void readArrayByte(unsigned int memAddress, byte *buffer, int length);
  void writeWord(int memAddress, char *word);
  void readWord(int memAddress, char *word);
  void clearPage(int memAddress);
private:
int address;
};

#endif /* _AT24XX_H_ */