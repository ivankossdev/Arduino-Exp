#include "drawShape.h"


void DrawShape::init() {
  Serial.begin(115200);

  if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  clearDisplay();
}


void DrawShape::dispCord(Shape& shape) {
  setTextSize(1);
  setTextColor(SSD1306_WHITE);
  setCursor(0, 0);

  char buf[32];
  snprintf(buf, sizeof(buf), "Pos: X%d Y%d", shape.xPos, shape.yPos);
  print(buf);
}


void DrawShape::drawFrame(const Shape& shape) {
  fillRect(shape.xPos, shape.yPos, SIZE_SHAPE, SIZE_SHAPE, WHITE);
}

void DrawShape::clearScreen() {
  fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
}
