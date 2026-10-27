#include "matr.h"
#include "EventTimer.h"
#include "input.h"
#include "buttons.h"
#include "accel.h"
#include "config.h"

enum ControlMode { MODE_AUTO, MODE_BUTTONS, MODE_ACCEL };

static EventTimer inputTimer(10);
static EventTimer stepTimer(50);

static ControlMode   mode = MODE_BUTTONS;
static Buttons       buttons;
static Accelerometer accel;

static const char* modeName(ControlMode m) {
  switch (m) {
    case MODE_AUTO:    return "AUTO";
    case MODE_BUTTONS: return "BUTTONS";
    case MODE_ACCEL:   return "ACCEL";
  }
  return "?";
}

static void logStep(const DotState& s) {
  Serial.print("pos ");
  Serial.print(s.pos);
  Serial.print(" (r");
  Serial.print(s.row);
  Serial.print(", c");
  Serial.print(s.col);
  Serial.print(") dir ");
  Serial.println(directionName(s.dir));
}

/* ---------- Переключение режимов ---------- */

static void enterMode(ControlMode m) {
  mode = m;
  Serial.print("mode: ");
  Serial.println(modeName(mode));

  if (mode == MODE_AUTO) {
    // Автопилот: первый шаг сразу, дальше — по stepTimer
    DotState s = dotStep();
    logStep(s);
    stepTimer.start();
  } else {
    stepTimer.stop();
    // В MANUAL-режимах: не давать фантомный шаг от зажатого ввода
    if (mode == MODE_BUTTONS) buttons.suppressUntilNeutral();
    if (mode == MODE_ACCEL)   accel.suppressUntilNeutral();
  }
}

static void cycleMode() {
  ControlMode next = MODE_AUTO;
  switch (mode) {
    case MODE_AUTO:    next = MODE_BUTTONS; break;
    case MODE_BUTTONS: next = MODE_ACCEL;   break;
    case MODE_ACCEL:   next = MODE_AUTO;    break;
  }
  enterMode(next);
}

/* ---------- Событие → направление ---------- */

static DirectionOfMovement dirFromEvent(IInput::Event ev) {
  switch (ev) {
    case IInput::EV_UP:    return UP;
    case IInput::EV_DOWN:  return DOWN;
    case IInput::EV_LEFT:  return LEFT;
    case IInput::EV_RIGHT: return RIGHT;
    default:               return STANDSTILL;
  }
}

/* ---------- Тик опроса ввода ---------- */

static void onInputTick() {
  // Кнопки опрашиваем ВСЕГДА: EV_MODE должен срабатывать
  // в любом режиме, включая ACCEL.
  IInput::Event btnEv = buttons.poll();

  if (btnEv == IInput::EV_MODE) {
    cycleMode();
    dotVisualTick();
    return;
  }

  // Направление — от активного источника
  IInput::Event dirEv = IInput::EV_NONE;

  switch (mode) {
    case MODE_BUTTONS:
      dirEv = btnEv;         // уже опросили выше — переиспользуем
      break;
    case MODE_ACCEL:
      dirEv = accel.poll();
      break;
    case MODE_AUTO:
      break;                 // автопилот сам двигает точку
  }

  if (dirEv != IInput::EV_NONE) {
    DotState s = dotMoveDirection(dirFromEvent(dirEv));
    logStep(s);
  }

  dotVisualTick();
}

/* ---------- Тик автопилота ---------- */

static void onStepTick() {
  if (mode != MODE_AUTO) return;
  DotState s = dotStep();
  logStep(s);
}

/* ---------- setup / loop ---------- */

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  buttons.begin();
  accel.begin();            // датчик калибруется один раз при старте
  matrInit(3);
  dotInit(START_POS);

  Serial.print("=== BOOT === mode: ");
  Serial.println(modeName(mode));

  inputTimer.setCallback(onInputTick);
  stepTimer.setCallback(onStepTick);
}

void loop() {
  inputTimer.tick();
  stepTimer.tick();
}