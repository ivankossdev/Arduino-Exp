#include "mShape.h"

void MShape::init() {
  Serial.begin(115200);

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
  fillRect(0, 0, 126, 16, BLACK);
  setTextSize(1);
  setTextColor(SSD1306_WHITE);
  setCursor(0, 0);
  
  char buf[32];
  sprintf(buf, "Position X%d Y%d", xPos, yPos);
  print(buf);
}

void MShape::movement(int16_t x, int16_t y) {
  xPos = x; yPos = y; 

  clearSh(off);
  fillRect(x, y, 10, 10, WHITE);
}

void MShape::clearSh(Display dsp) {
  fillRect(xOldPos, yOldPos, 10, 10, BLACK);
  xOldPos = xPos;
  yOldPos = yPos;
}

void MShape::setDefaultXPos() {
  xPos = 0;
}

void MShape::setDefaultYPos() {
  yPos = 16;
}

void MShape::clear(){
  fillRect(xPos, yPos, 10, 10, BLACK);
}
