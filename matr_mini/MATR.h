#ifndef __MATR_H
#define __MATR_H

#include "Arduino.h"

class MATR {
public:
  MATR(uint8_t _intensity = 4, byte dataPin = D7, byte clockPin = D5);
  uint8_t intensity;
  volatile uint8_t displayArray[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  void showDisplay();
  void clearDisplay();
  void insertToDisplay(uint8_t adr,uint8_t data);

protected:
  void sendCommand(byte led);
  void sendData(byte add, byte data);
  void send(byte data);
  byte dataPin;
  byte clockPin;
};

#endif