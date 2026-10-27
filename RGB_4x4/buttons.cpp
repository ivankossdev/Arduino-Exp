#include "buttons.h"
#include "config.h"

static bool isPressed(int pin) {
#if BTN_ACTIVE_LOW
  return digitalRead(pin) == LOW;
#else
  return digitalRead(pin) == HIGH;
#endif
}

void Buttons::begin() {
#if BTN_ACTIVE_LOW
  pinMode(BTN_UP,    INPUT_PULLUP);
  pinMode(BTN_DOWN,  INPUT_PULLUP);
  pinMode(BTN_LEFT,  INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_MODE,  INPUT_PULLUP);
#else
  pinMode(BTN_UP,    INPUT);
  pinMode(BTN_DOWN,  INPUT);
  pinMode(BTN_LEFT,  INPUT);
  pinMode(BTN_RIGHT, INPUT);
  pinMode(BTN_MODE,  INPUT);
#endif

  m_modeRaw    = isPressed(BTN_MODE);
  m_modeStable = m_modeRaw;
  m_modeChange = millis();

  m_dirRaw    = -1;
  m_dirChange = millis();
  m_dirLocked = false;
}

IInput::Event Buttons::poll() {
  const unsigned long DEBOUNCE = 25;
  unsigned long now = millis();

  // --- MODE: фронт ---
  bool modeNow = isPressed(BTN_MODE);
  if (modeNow != m_modeRaw) {
    m_modeRaw    = modeNow;
    m_modeChange = now;
  }
  if ((now - m_modeChange) >= DEBOUNCE && modeNow != m_modeStable) {
    m_modeStable = modeNow;
    if (m_modeStable) return EV_MODE;
  }

  // --- Направления: latch ---
  int dirNow = -1;
  if      (isPressed(BTN_UP))    dirNow = BTN_UP;
  else if (isPressed(BTN_DOWN))  dirNow = BTN_DOWN;
  else if (isPressed(BTN_LEFT))  dirNow = BTN_LEFT;
  else if (isPressed(BTN_RIGHT)) dirNow = BTN_RIGHT;

  if (dirNow != m_dirRaw) {
    m_dirRaw    = dirNow;
    m_dirChange = now;
  }

  if (m_dirLocked) {
    if (dirNow == -1 && (now - m_dirChange) >= DEBOUNCE) {
      m_dirLocked = false;
    }
    return EV_NONE;
  }

  if (dirNow == -1) return EV_NONE;
  if ((now - m_dirChange) < DEBOUNCE) return EV_NONE;

  m_dirLocked = true;
  switch (dirNow) {
    case BTN_UP:    return EV_UP;
    case BTN_DOWN:  return EV_DOWN;
    case BTN_LEFT:  return EV_LEFT;
    case BTN_RIGHT: return EV_RIGHT;
  }
  return EV_NONE;
}

void Buttons::suppressUntilNeutral() {
  m_dirLocked = true;
  m_dirRaw    = -1;
  m_dirChange = millis();
}