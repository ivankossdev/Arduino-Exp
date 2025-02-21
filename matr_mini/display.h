#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "MATR.h"
#include "Arduino.h"
#include "MemHandler.h"

class Display : public MATR{
  
  public: 
    using MATR::MATR;
    Memory memory;
    void inserDigiatl(uint8_t dig);
    void inserDigiatl(uint8_t dig, int sh);
    void insertShape(uint8_t *array);
    void insertShiftShape(uint8_t *array, int sh);
    void scrollLeft(int scr);
    void scrollRight(int scr);
    void scrollRightLoop();
    void printChar(char ch);
    void scrollLeftString(unsigned long score);

  protected:
    uint8_t reverseChar(uint8_t ch);
    uint8_t *getAsciiArray(uint8_t ch);
    uint8_t *getCyrillicArray(unsigned short int ch);
    void insertToDispBuffer(uint8_t *array);
    void insertToDispBuffer(uint8_t *array, int sh);
    
  private:
    int getWidthShape(uint8_t * fig);
    char * createString(char myInt);
    void sliceShapeByCordY(uint8_t *shape, int position);
    uint8_t *buffer = (uint8_t *)calloc(8, sizeof(uint8_t));
};

#endif