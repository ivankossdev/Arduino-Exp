#include "moveshape.h"

void MoveShape::init() {
  Serial.begin(115200);

  if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  this->xOldPos = 0;
  this->yOldPos = 0;
  clearDisplay();
  display();
}

void MoveShape::dispCord(int16_t x, int16_t y) {
  fillRect(0, 0, 126, 16, BLACK);
  setTextSize(1);
  setTextColor(SSD1306_WHITE);
  setCursor(0, 0);
  printf("Position X%d Y%d", x, y);
  display();
}

void MoveShape::movement(int16_t x, int16_t y) {
  if (this->xOldPos != x || this->yOldPos != y)
    fillRect(x - 1, y - 1, 10, 10, BLACK);
  this->xOldPos = x; this->yOldPos = y;

  fillRect(x, y, 10, 10, WHITE);
  display();
}

void MoveShape::clearSh(int16_t x, int16_t y) {
  fillRect(x - 1, y - 1, 10, 10, BLACK);
}
