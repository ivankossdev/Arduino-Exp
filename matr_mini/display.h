#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "MATR.h"
#include "Arduino.h"

class Display : public MATR{
  
  public: 
    using MATR::MATR;
    void inserDigiatl(uint8_t dig);
    void inserDigiatl(uint8_t dig, int sh);
    void insertShape(uint8_t *array);
    void insertShiftShape(uint8_t *array, int sh);
    void scrollLeft(int scr);
    void scrollRight(int scr);
    void scrollRightLoop();
    void printChar(char ch);
    void scrollLeftString(char *str, unsigned long score);

  protected:
    uint8_t reverseChar(uint8_t ch);
    uint8_t *getSimvolArray(uint8_t ch);
    void insertToDispBuffer(uint8_t *array);
    void insertToDispBuffer(uint8_t *array, int sh);
  private:
    int getWidthShape(char * fig);
    char * createString(char myInt);
    void printSerialShape(uint8_t *fig);

};

#endif