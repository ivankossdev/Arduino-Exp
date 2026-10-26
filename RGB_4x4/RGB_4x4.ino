#include "matr.h"
#include "EventTimer.h"

static EventTimer stepTimer(1000);

/* Адаптер: EventTimer ждёт void(*)(), а dotStep() возвращает DotState. */
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
  matrInit(3);
  stepTimer.setCallback(onStep);
}

void loop() {
  stepTimer.tick();
}