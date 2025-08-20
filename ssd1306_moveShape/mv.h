#ifndef _MV_H
#define _MV_H
#include "Arduino.h"
#include "mShape.h"

#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64  

#define OLED_RESET -1        
#define SCREEN_ADDRESS 0x3C  

const int16_t MAX_XPOS = 117;
const int16_t MIN_XPOS = 0;
const int16_t MAX_YPOS = 54;
const int16_t MIN_YPOS = 16;

static MShape MShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void mshInit();
void mvCord(int16_t x, int16_t y);
void mvRight();
void mvDown();
void mvLeft();
void mvUp();
void downAndRight();
void upAndRigth();
void downAndLeft();
void upAndLeft();
void clear();

#endif 