#ifndef _MV_H
#define _MV_H
#include "Arduino.h"
#include "drawShape.h"
#include "config.h"


extern DrawShape mShape;
void mshInit();
void mvCord(int16_t x, int16_t y);
void mvRight();
void mvDown();
void mvLeft();
void mvUp();
void downAndRight();
void upAndRight();
void downAndLeft();
void upAndLeft();
void stepTowardsTarget();

#endif 