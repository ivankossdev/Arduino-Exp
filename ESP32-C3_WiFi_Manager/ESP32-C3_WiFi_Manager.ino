#include "Menu.h"

Menu menu; 

void setup() {
  Serial.begin(115200);
  menu.begin();
}

void loop() {
  menu.update();
}