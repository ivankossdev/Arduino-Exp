#include "matr.h"
#include "EventTimer.h"

static EventTimer stepTimer(80);

/* Адаптер: EventTimer ждёт void(*)(), а dotStep() возвращает DotState.
 * Здесь — место, где решается, что делать с результатом шага. */
static void onStep() {
  DotState s = dotStep();

  Serial.print("pos ");
  Serial.print(s.pos);
  Serial.print(" (r");
  Serial.print(s.row);
  Serial.print(", c");
  Serial.print(s.col);
  Serial.print(") dir ");
  Serial.println(directionName(s.dir));
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));   // иначе последовательность одинаковая при каждом ресете
  matrInit(3);
  stepTimer.setCallback(dotStep);
}

void loop() {
  stepTimer.tick();
}