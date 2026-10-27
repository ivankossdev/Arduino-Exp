#include "matr.h"
#include "EventTimer.h"
#include "buttons.h"
#include "config.h"

enum ControlMode { MODE_MANUAL, MODE_AUTO };

static EventTimer inputTimer(10);    // опрос кнопок 100 Гц
static EventTimer stepTimer(50);   // авто-шаг 1 Гц

static ControlMode mode = MODE_MANUAL;
static Buttons     buttons;

/* ---------- Вывод ---------- */

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

/* ---------- Режимы ---------- */

static void switchMode() {
  mode = (mode == MODE_MANUAL) ? MODE_AUTO : MODE_MANUAL;

  Serial.print("mode: ");
  Serial.println(mode == MODE_AUTO ? "AUTO" : "MANUAL");

  if (mode == MODE_AUTO) {
    DotState s = dotStep();   // первый шаг сразу, без секунды ожидания
    logStep(s);
    stepTimer.start();
  } else {
    stepTimer.stop();
    buttons.lockUntilRelease();   // зажатая кнопка не даст фантомный шаг
  }
}

/* ---------- Событие → направление ---------- */

static DirectionOfMovement dirFromEvent(Buttons::Event ev) {
  switch (ev) {
    case Buttons::EV_UP:    return UP;
    case Buttons::EV_DOWN:  return DOWN;
    case Buttons::EV_LEFT:  return LEFT;
    case Buttons::EV_RIGHT: return RIGHT;
    default:                return STANDSTILL;
  }
}

/* ---------- Тики ---------- */

static void onInputTick() {
  Buttons::Event ev = buttons.poll();

  dotVisualTick();

  if (ev == Buttons::EV_NONE) return;

  if (ev == Buttons::EV_MODE) {
    switchMode();
    return;
  }

  if (mode != MODE_MANUAL) return;

  DotState s = dotMoveDirection(dirFromEvent(ev));
  logStep(s);
}

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
  matrInit(3);
  dotInit(START_POS);

  Serial.println("=== BOOT === mode: MANUAL");

  inputTimer.setCallback(onInputTick);
  stepTimer.setCallback(onStepTick);
}

void loop() {
  inputTimer.tick();
  stepTimer.tick();
}