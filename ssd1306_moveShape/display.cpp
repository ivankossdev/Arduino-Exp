#include "display.h"

void Display::init() {
  Serial.begin(115200);

  if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  clearDisplay();
  display();
}

void Display::blinkRect(int16_t x, int16_t y, unsigned long t) {
  drawRect(x, y, 30, 30, WHITE);
  display();
  delay(t);

  drawRect(x, y, 30, 30, BLACK);
  display();
  delay(t);
}