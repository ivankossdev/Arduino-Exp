#include <Arduino.h>

const unsigned long PERIOD_MS = 1000;

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long last = 0;
  unsigned long now = millis();
  if (now - last < PERIOD_MS) return;
  last = now;

  // Заглушка температуры: медленная "волна" ~20..30 °C
  float t = 25.0f + 5.0f * sin(now / 3000.0f);

  // Реальное значение АЦП
  int a = analogRead(A0);

  Serial.print("T=");
  Serial.print(t, 1);
  Serial.print(";A=");
  Serial.print(a);
  Serial.print('\n');
}