// accel.cpp
#include "accel.h"
#include "config.h"

void Accelerometer::begin() {
  // TODO: I2C init, настройка датчика
}

IInput::Event Accelerometer::poll() {
  // TODO: чтение датчика → определение наклона → шаг
  //   - если наклон вверх и прошло STEP_COOLDOWN_MS — return EV_UP;
  //   - иначе EV_NONE.
  return EV_NONE;
}

void Accelerometer::suppressUntilNeutral() {
  // TODO: сброс «наклона», чтобы первый шаг не сработал от старого состояния
}