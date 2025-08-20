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
  display();
}

void MShape::dispCord() {
  fillRect(0, 0, 126, 16, BLACK);
  setTextSize(1);
  setTextColor(SSD1306_WHITE);
  setCursor(0, 0);
  printf("Position X%d Y%d", xPos, yPos);
  display();
}

void MShape::movement(int16_t x, int16_t y) {
  xPos = x; yPos = y; 

  clearSh(off);
  fillRect(x, y, 10, 10, WHITE);
  display();
}

void MShape::clearSh(Display dsp) {

  // Движение вниз и вправо
  if (this->xOldPos < xPos && this->yOldPos < yPos) {
    fillRect(xPos - 1, yPos - 1, 10, 10, BLACK);
  }

  // Движение вниз и влево
  if (this->xOldPos > xPos && this->yOldPos < yPos) {
    fillRect(xPos + 1, yPos - 1, 10, 10, BLACK);
  }
  
  // Движение вверх и вправо
  if (this->xOldPos< xPos && this->yOldPos > yPos) {
    fillRect(xPos - 1, yPos + 1, 10, 10, BLACK);
  }

  // Движение вверх и влево
  if (this->xOldPos > xPos && this->yOldPos > yPos) {
    fillRect(xPos + 1, yPos + 1, 10, 10, BLACK);
  }

  // Движение вправо
  if (this->xOldPos < xPos && this->yOldPos == yPos) {
    fillRect(xPos - 1, yPos, 10, 10, BLACK);
  }

  // Дижение влево
  if (this->xOldPos > xPos && this->yOldPos == yPos) {
    fillRect(xPos + 1, yPos, 10, 10, BLACK);
  }

  // Движение вниз
  if (this->yOldPos < yPos) {
    fillRect(xPos, yPos - 1, 10, 10, BLACK);
  }

  // Движение вверх
  if (this->yOldPos > yPos) {
    fillRect(xPos, yPos + 1, 10, 10, BLACK);
  }

  Serial.printf("xOldPos %d, yOldPos %d, xPos %d, yPos %d \n", xOldPos, yOldPos, xPos, yPos);
  this->xOldPos = xPos;
  this->yOldPos = yPos;

  if (dsp == on) {
    display();
  }
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









