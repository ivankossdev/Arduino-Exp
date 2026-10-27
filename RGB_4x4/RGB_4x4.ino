#include "matr.h"
#include "EventTimer.h"
#include "input.h"
#include "buttons.h"
#include "config.h"
// #include "accel.h"   // ← раскомментируешь, когда подключишь датчик

enum ControlMode { MODE_MANUAL, MODE_AUTO };

static EventTimer inputTimer(10);
static EventTimer stepTimer(1000);

static ControlMode mode = MODE_MANUAL;

/* ---- Точка подключения конкретного класса ---- *
 * Чтобы сменить устройство ввода — меняешь эти две строки:
 *   static Buttons buttons;      → static Accelerometer buttons;
 *   static IInput* input = &buttons; */
static Buttons buttons;
static IInput* input = &buttons;

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

static void switchMode() {
  mode = (mode == MODE_MANUAL) ? MODE_AUTO : MODE_MANUAL;

  Serial.print("mode: ");
  Serial.println(mode == MODE_AUTO ? "AUTO" : "MANUAL");

  if (mode == MODE_AUTO) {
    DotState s = dotStep();
    logStep(s);
    stepTimer.start();
  } else {
    stepTimer.stop();
    input->suppressUntilNeutral();
  }
}

static DirectionOfMovement dirFromEvent(IInput::Event ev) {
  switch (ev) {
    case IInput::EV_UP:    return UP;
    case IInput::EV_DOWN:  return DOWN;
    case IInput::EV_LEFT:  return LEFT;
    case IInput::EV_RIGHT: return RIGHT;
    default:               return STANDSTILL;
  }
}

static void onInputTick() {
  IInput::Event ev = input->poll();
  dotVisualTick();

  if (ev == IInput::EV_NONE) return;

  if (ev == IInput::EV_MODE) {
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

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  input->begin();
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