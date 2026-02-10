#include <Bounce2.h>

#define BUTTON_PIN 4  // если есть кнопка

Bounce debouncer = Bounce();

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(25); // интервал антидребезга в мс
}

void loop() {
  debouncer.update();
  
  if (debouncer.fell()) {
    // Кнопка нажата (с антидребезгом)
    Serial.println("Press button");
  }
}