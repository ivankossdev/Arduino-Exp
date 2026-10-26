#include "matr.h"
#include "EventTimer.h"
#include "config.h"

enum ControlMode { MODE_MANUAL, MODE_AUTO };

static EventTimer inputTimer(10);    // опрос кнопок 100 Гц
static EventTimer stepTimer(50);   // авто-шаг 1 Гц

static ControlMode mode = MODE_MANUAL;

/* ---------- Чтение кнопок ---------- */

static bool isPressed(int pin) {
#if BTN_ACTIVE_LOW
  return digitalRead(pin) == LOW;
#else
  return digitalRead(pin) == HIGH;
#endif
}

/* Первая нажатая по приоритету UP>DOWN>LEFT>RIGHT, или -1. */
static int readRawDirPin() {
  if (isPressed(BTN_UP))    return BTN_UP;
  if (isPressed(BTN_DOWN))  return BTN_DOWN;
  if (isPressed(BTN_LEFT))  return BTN_LEFT;
  if (isPressed(BTN_RIGHT)) return BTN_RIGHT;
  return -1;
}

static DirectionOfMovement dirForPin(int pin) {
  if (pin == BTN_UP)    return UP;
  if (pin == BTN_DOWN)  return DOWN;
  if (pin == BTN_LEFT)  return LEFT;
  if (pin == BTN_RIGHT) return RIGHT;
  return STANDSTILL;
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

/* ---------- Состояние направляющих кнопок ---------- */

static bool          dirLocked     = false;
static int           dirLastRaw    = -1;
static unsigned long dirLastChange = 0;

/* ---------- Состояние кнопки MODE ---------- */

static bool          modeRaw       = false;
static bool          modeStable    = false;
static unsigned long modeChange    = 0;

/* ---------- Смена режима ---------- */

static void switchMode() {
  mode = (mode == MODE_MANUAL) ? MODE_AUTO : MODE_MANUAL;

  Serial.print("mode: ");
  Serial.println(mode == MODE_AUTO ? "AUTO" : "MANUAL");

  // Сбрасываем состояние направляющих кнопок, чтобы при возврате
  // в MANUAL не сработал «висящий» шаг от кнопки, которую держали.
  dirLocked  = (mode == MODE_MANUAL);  // в MANUAL ждём первого отпускания
  dirLastRaw = -1;

  if (mode == MODE_AUTO) {
    DotState s = dotStep();   // сразу первый шаг — без секунды ожидания
    logStep(s);
    stepTimer.start();
  } else {
    stepTimer.stop();
  }
}

/* ---------- Тик кнопок (10 мс) ---------- */

static void onInputTick() {
  const unsigned long DEBOUNCE = 25;

  /* --- Кнопка MODE: фронт нажатия --- */
  bool raw = isPressed(BTN_MODE);
  if (raw != modeRaw) {
    modeRaw    = raw;
    modeChange = millis();
  }
  if ((millis() - modeChange) >= DEBOUNCE && raw != modeStable) {
    modeStable = raw;
    if (modeStable) {
      switchMode();
    }
  }

  /* --- Направляющие кнопки: только в MANUAL --- */
  if (mode != MODE_MANUAL) return;

  int dirPin = readRawDirPin();
  if (dirPin != dirLastRaw) {
    dirLastRaw    = dirPin;
    dirLastChange = millis();
  }

  // Захват: пока кнопка не отпущена (с антидребезгом) — новых шагов нет
  if (dirLocked) {
    if (dirPin == -1 && (millis() - dirLastChange) >= DEBOUNCE) {
      dirLocked = false;
    }
    return;
  }

  if (dirPin == -1) return;
  if ((millis() - dirLastChange) < DEBOUNCE) return;

  DotState s = dotMoveDirection(dirForPin(dirPin));
  dirLocked = true;
  logStep(s);
}

/* ---------- Тик авто-шага ---------- */

static void onStepTick() {
  // stepTimer запущен только в AUTO, так что проверка не нужна,
  // но оставим на всякий случай — защита от будущих правок.
  if (mode != MODE_AUTO) return;

  DotState s = dotStep();
  logStep(s);
}

/* ---------- setup / loop ---------- */

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));

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