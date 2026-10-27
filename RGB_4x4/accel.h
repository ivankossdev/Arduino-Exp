// accel.h
#pragma once
#include "input.h"

class Accelerometer : public IInput {
public:
  void begin() override;
  Event poll() override;
  void suppressUntilNeutral() override;

private:
  unsigned long m_lastStep = 0;
  // сюда пойдут поля датчика, фильтра, последнего наклона
};