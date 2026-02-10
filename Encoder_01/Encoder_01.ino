#include <Encoder.h>

// Создаем объект энкодера (пины S1, S2)
Encoder myEncoder(2, 3);

void setup() {
  Serial.begin(9600);
  Serial.println("Энкодер с антидребезгом");
}

void loop() {
  static long oldPosition = 0;
  long newPosition = myEncoder.read();
  
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.print("Позиция: ");
    Serial.println(newPosition);
  }
}