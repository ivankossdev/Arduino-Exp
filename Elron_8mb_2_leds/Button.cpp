#include "Button.h"

Button::Button(uint8_t pin, uint8_t activeLevel, unsigned long debounceMs)
  : pin(pin),
    activeLevel(activeLevel),
    debounceMs(debounceMs),
    longPressMs(2000),                             // дефолт, переопределяется
    rawReading(activeLevel == HIGH ? LOW : HIGH),  // «отпущено» = не activeLevel
    stableReading(activeLevel == HIGH ? LOW : HIGH),
    lastRawChange(0),
    pressStart(0),
    longPressFired(false),
    veryLongPressFired(false),
    onShortPress(nullptr),
    onLongPress(nullptr),
    onVeryLongPress(nullptr),
    onPressStart(nullptr) {}

void Button::setShortPressCallback(ButtonCallback cb) {
  onShortPress = cb;
}
void Button::setLongPressCallback(ButtonCallback cb) {
  onLongPress = cb;
}
void Button::setVeryLongPressCallback(ButtonCallback cb) {
  onVeryLongPress = cb;
}
void Button::setLongPressMs(unsigned long ms) {
  longPressMs = ms;
}
void Button::setVeryLongPressMs(unsigned long ms) {
  veryLongPressMs = ms;
}
void Button::setPressStartCallback(ButtonCallback cb) {
  onPressStart = cb;
}

void Button::tick() {
  unsigned long now = millis();
  bool reading = digitalRead(pin);

  // 1. Запоминаем момент последнего изменения «сырого» сигнала
  if (reading != rawReading) {
    rawReading = reading;
    lastRawChange = now;
  }

  // 2. Принимаем состояние, только если оно стабильно > debounceMs
  if (reading != stableReading && (now - lastRawChange) >= debounceMs) {
    stableReading = reading;

    if (stableReading == activeLevel) {
      pressStart = now;
      longPressFired = false;
      veryLongPressFired = false;
      if (onPressStart != nullptr) onPressStart();
      Serial.println("[BTN] down");
    } else {
      unsigned long held = now - pressStart;
      if (!longPressFired && held < longPressMs) {
        if (onShortPress != nullptr) onShortPress();
      }
      longPressFired = false;
    }
  }

  // 3. Длинное удержание
  if (stableReading == activeLevel && !longPressFired && pressStart != 0
      && (now - pressStart) >= longPressMs) {
    longPressFired = true;
    if (onLongPress != nullptr) onLongPress();
  }

  // 4. Сверхдлинное удержание
  if (stableReading == activeLevel && !veryLongPressFired && pressStart != 0
      && (now - pressStart) >= veryLongPressMs) {
    veryLongPressFired = true;
    if (onVeryLongPress != nullptr) onVeryLongPress();
  }
}
