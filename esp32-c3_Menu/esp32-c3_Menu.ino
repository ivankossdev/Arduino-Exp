#include "Menu.h"
#define LED_PIN 8

// Создаём экземпляр меню для встроенного светодиода (пин 2)
Menu myMenu(LED_PIN);

void setup() {
  myMenu.begin();
}

void loop() {
  myMenu.update();
  delay(100);
}
