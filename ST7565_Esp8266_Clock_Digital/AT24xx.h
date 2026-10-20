#ifndef _AT24XX_H_
#define _AT24XX_H_
#include <Arduino.h>
#include <Wire.h>

#define EEPROM_I2C_ADDRESS 0x57

class At24{
public:
  At24(uint8_t address_);
  void writeByte(unsigned int memAddress, byte data);
  byte readByte(unsigned int memAddress);
  void writeArrayByte(unsigned int memAddress, byte *data, int length);
  void readArrayByte(unsigned int memAddress, byte *buffer, int length);
  // Читает до `length` байт и возвращает указатель на внутренний
  // нуль-терминированный буфер. Действителен до следующего вызова readString().
  const char* readString(unsigned int memAddress, int length);

private:
  uint8_t address;
  byte    buffer[65] = { '\0' };
};

#endif /* _AT24XX_H_ */