#include "matr.h"
#include "EventTimer.h"
#include "config.h"

static EventTimer inputTimer(10);   // опрос 100 Гц

/* ---------- Чтение сырых кнопок ---------- */

static bool isPressed(int pin) {
#if BTN_ACTIVE_LOW
  return digitalRead(pin) == LOW;
#else
  return digitalRead(pin) == HIGH;
#endif
}

/* Возвращает пин первой нажатой кнопки по приоритету UP>DOWN>LEFT>RIGHT,
 * или -1, если ничего не нажато. */
static int readRawPin() {
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

/* ---------- Тик опроса ---------- */

static void onTick() {
  static int           lastRaw    = -1;
  static unsigned long lastChange = 0;
  static bool          locked     = false;

  const unsigned long DEBOUNCE = 25;

  int raw = readRawPin();
  if (raw != lastRaw) {
    lastRaw    = raw;
    lastChange = millis();
  }

  // Захват активен: ждём, пока ВСЕ кнопки отпустят (с антидребезгом)
  if (locked) {
    if (raw == -1 && (millis() - lastChange) >= DEBOUNCE) {
      locked = false;
    }
    return;
  }

  // Ничего не нажато
  if (raw == -1) return;

  // Дебаунс: нажатие подтверждается, если пин стабилен уже DEBOUNCE мс
  if ((millis() - lastChange) < DEBOUNCE) return;

  // Сработало — делаем шаг и захватываем кнопку
  DirectionOfMovement dir = dirForPin(raw);
  DotState s = dotMoveDirection(dir);
  locked = true;

  Serial.print("pos ");
  Serial.print(s.pos);
  Serial.print(" (r");
  Serial.print(s.row);
  Serial.print(", c");
  Serial.print(s.col);
  Serial.print(") dir ");
  Serial.println(directionName(s.dir));
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
#else
  pinMode(BTN_UP,    INPUT);
  pinMode(BTN_DOWN,  INPUT);
  pinMode(BTN_LEFT,  INPUT);
  pinMode(BTN_RIGHT, INPUT);
#endif

  matrInit(3);
  dotInit(START_POS);

  inputTimer.setCallback(onTick);
}

void loop() {
  inputTimer.tick();
}