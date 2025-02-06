#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "MATR.h"
#include "Arduino.h"

class Display : public MATR{
  public: 
  using MATR::MATR;
  uint8_t ReverseChar(uint8_t ch);
  uint8_t *GetSimvolArray(uint8_t ch);
  void insertToDispBuffer(uint8_t *array);
};

#endif