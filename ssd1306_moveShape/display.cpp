#include "display.h"

void Display::Init(){
  Serial.begin(115200);

  if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  clearDisplay();
  display();
}

void Display::insRect(int16_t x, int16_t y){
  drawRect(x, y, 30, 30, WHITE);
  display();
}