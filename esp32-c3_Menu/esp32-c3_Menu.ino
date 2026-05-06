#include "Menu.h"
#define LED_PIN 8

Menu myMenu(LED_PIN);

void setup() {
  myMenu.begin();
}

void loop() {
  myMenu.update();
  delay(100);
}
