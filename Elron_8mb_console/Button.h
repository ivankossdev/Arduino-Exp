#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

typedef void (*ButtonCallback)();

class Button {
public:
  // pin        — номер пина
  // activeLevel — HIGH или LOW (уровень, соответствующий нажатию)
  // debounceMs  — окно антидребезга
  Button(uint8_t pin, uint8_t activeLevel, unsigned long debounceMs = 50);

  void setShortPressCallback(ButtonCallback cb);
  void setLongPressCallback(ButtonCallback cb);
  void setLongPressMs(unsigned long ms);

  void tick();   // вызывать из loop()

private:
  uint8_t       pin;
  uint8_t       activeLevel;
  unsigned long debounceMs;
  unsigned long longPressMs;

  bool          rawReading;
  bool          stableReading;
  unsigned long lastRawChange;
  unsigned long pressStart;
  bool          longPressFired;

  ButtonCallback onShortPress;
  ButtonCallback onLongPress;
};

#endif // BUTTON_H