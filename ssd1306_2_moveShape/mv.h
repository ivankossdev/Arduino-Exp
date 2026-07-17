#ifndef _MV_H
#define _MV_H
#include "Arduino.h"
#include "drawShape.h"
#include "config.h"

int16_t MAX_XPOS(Shape& mShape);
int16_t MAX_YPOS(Shape& mShape);
void stepTowardsTarget(Shape& mShape);

#endif 