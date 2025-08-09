#include "moveshape.h"

void MoveShape::init() {
  Serial.begin(115200);

  if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  clearDisplay();
  display();
}

void MoveShape::shovPosition(int16_t x, int16_t y) {
  fillRect(0, 0, 126, 16, BLACK);
  setTextSize(1);
  setTextColor(SSD1306_WHITE);
  setCursor(0, 0);
  printf("Position X%d Y%d", x, y);
  display();
}

void MoveShape::movement(int16_t x, int16_t y) {
  if (x != 0)
    fillRect(x - 1, y, 10, 10, BLACK);
  fillRect(x, y, 10, 10, WHITE);
  display();
}

void MoveShape::clearSh(int16_t x, int16_t y) {
  if (x != 0)
    fillRect(x - 1, y, 10, 10, BLACK);
}
