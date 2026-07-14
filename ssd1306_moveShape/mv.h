#ifndef _MV_H
#define _MV_H
#include "Arduino.h"
#include "mShape.h"
#include "config.h"


extern MShape mShape;
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
void clear();

#endif 