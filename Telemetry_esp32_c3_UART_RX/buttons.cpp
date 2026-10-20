#include "buttons.h"
#include "config.h"

struct Button {
  uint8_t  pin;
  bool     stable;
  bool     lastRead;
  unsigned long lastChange;
};

static Button btn1 = { BTN1_PIN, false, false, 0 };
static Button btn2 = { BTN2_PIN, false, false, 0 };

static bool readPressed(uint8_t pin) {
#if BTN_ACTIVE_LOW
  return digitalRead(pin) == LOW;
#else
  return digitalRead(pin) == HIGH;
#endif
}

// true ровно один раз в момент нажатия
static bool checkPressed(Button &b) {
  bool pressed = readPressed(b.pin);

  if (pressed != b.lastRead) {
    b.lastRead = pressed;
    b.lastChange = millis();
  }

  if (millis() - b.lastChange >= BTN_DEBOUNCE_MS) {
    if (pressed != b.stable) {
      b.stable = pressed;
      if (b.stable) return true;
    }
  }
  return false;
}

void buttonsInit() {
  pinMode(BTN1_PIN, INPUT);  // внешняя подтяжка
  pinMode(BTN2_PIN, INPUT);
}

ButtonEvent buttonsPoll() {
  if (checkPressed(btn1)) return BTN1_PRESSED;
  if (checkPressed(btn2)) return BTN2_PRESSED;
  return BTN_NONE;
}