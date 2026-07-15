#include "mShape.h"


void MShape::init() {
  Serial.begin(115200);

  speedX = 1;
  speedY = 1;

  if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  this->xOldPos = 0;
  this->yOldPos = 0;

  this->xPos = 0;
  this->yPos = 16;

  clearDisplay();
  updateDisplay();
}


void MShape::dispCord() {
  setTextSize(1);
  setTextColor(SSD1306_WHITE);
  setCursor(0, 0);

  char buf[32];
  snprintf(buf, sizeof(buf), "Pos: X%d Y%d", xPos, yPos);
  print(buf);
}


void MShape::movement(int16_t x, int16_t y) {
  xPos = x;
  yPos = y;

  clearSh();
  fillRect(x, y, 10, 10, WHITE);
}


void MShape::clearSh() {
  fillRect(xOldPos, yOldPos, 10, 10, BLACK);
  xOldPos = xPos;
  yOldPos = yPos;
}


void MShape::drawFrame() {

  fillRect(0, 0, 128, 16, BLACK);
  fillRect(xOldPos, yOldPos, SIZE_SHAPE, SIZE_SHAPE, BLACK);
  xOldPos = xPos;
  yOldPos = yPos;

  fillRect(xPos, yPos, SIZE_SHAPE, SIZE_SHAPE, WHITE);

  dispCord();
  display();
}

