#ifndef __MATR_H
#define __MATR_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class MATR {
public:
  MATR(uint8_t _intensity = 4, byte dataPin = D7, byte clockPin = D5);
  uint8_t intensity;
  volatile uint8_t displayArray[8] = { 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  void showDisplay();
  void clearDisplay();
  void insertToDisplay(uint8_t data);

protected:
  void sendCommand(byte led);
  void sendData(byte add, byte data);
  void send(byte data);
  byte dataPin;
  byte clockPin;
};

#endif