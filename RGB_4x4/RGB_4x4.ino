#include "matr.h"
#include "EventTimer.h"
#include "input.h"
#include "buttons.h"
#include "accel.h"
#include "config.h"

enum ControlMode { MODE_AUTO, MODE_BUTTONS, MODE_ACCEL };

static EventTimer inputTimer(10);
static EventTimer stepTimer(100);

static ControlMode   mode = MODE_BUTTONS;
static Buttons       buttons;
static Accelerometer accel;

static int g_score = 0;

/* ---------- Имя режима (нужно и для лога, и потенциально для UI) ---------- */

static const char* modeName(ControlMode m) {
  switch (m) {
    case MODE_AUTO:    return "AUTO";
    case MODE_BUTTONS: return "BUTTONS";
    case MODE_ACCEL:   return "ACCEL";
  }
  return "?";
}

/* ---------- Обработка результата шага ----------
 * Счёт — всегда. Лог — только если включён. */

static void processStep(const DotState& s) {
  if (s.reachedTarget) g_score++;

#if LOG_ENABLED
  Serial.print("pos ");
  Serial.print(s.pos);
  Serial.print(" (r");
  Serial.print(s.row);
  Serial.print(", c");
  Serial.print(s.col);
  Serial.print(") dir ");
  Serial.print(directionName(s.dir));
  if (s.reachedTarget) {
    Serial.print("  GOAL! score=");
    Serial.print(g_score);
  }
  Serial.println();
#else
  (void)s;   // s используется только для лога — когда лог выключен, подавляем warning
#endif
}

/* ---------- Переключение режимов ---------- */

static void enterMode(ControlMode m) {
  mode = m;

#if LOG_ENABLED
  Serial.print("mode: ");
  Serial.println(modeName(mode));
#endif

  if (mode == MODE_AUTO) {
    processStep(dotStep());
    stepTimer.start();
  } else {
    stepTimer.stop();
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
  IInput::Event btnEv = buttons.poll();

  if (btnEv == IInput::EV_MODE) {
    cycleMode();
    dotVisualTick();
    return;
  }

  IInput::Event dirEv = IInput::EV_NONE;

  switch (mode) {
    case MODE_BUTTONS:
      dirEv = btnEv;
      break;
    case MODE_ACCEL:
      dirEv = accel.poll();
      break;
    case MODE_AUTO:
      break;
  }

  if (dirEv != IInput::EV_NONE) {
    processStep(dotMoveDirection(dirFromEvent(dirEv)));
  }

  dotVisualTick();
}

/* ---------- Тик автопилота ---------- */

static void onStepTick() {
  if (mode != MODE_AUTO) return;
  processStep(dotStep());
}

/* ---------- setup / loop ---------- */

void setup() {
#if LOG_ENABLED
  Serial.begin(9600);
#endif
  randomSeed(analogRead(A0));

  buttons.begin();
  accel.begin();
  matrInit(3);
  dotInit(START_POS);

#if LOG_ENABLED
  Serial.print("=== BOOT === mode: ");
  Serial.println(modeName(mode));
#endif

  inputTimer.setCallback(onInputTick);
  stepTimer.setCallback(onStepTick);
}

void loop() {
  inputTimer.tick();
  stepTimer.tick();
}