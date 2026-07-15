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


void DrawShape::movement(Shape& shape, int16_t x, int16_t y) {
  shape.xPos = x;
  shape.yPos = y;

  clearSh(shape);
  fillRect(x, y, 10, 10, WHITE);
}


void DrawShape::clearSh(Shape& shape) {
  fillRect(shape.xOldPos, shape.yOldPos, 10, 10, BLACK);
  shape.xOldPos = shape.xPos;
  shape.yOldPos = shape.yPos;
}


void DrawShape::drawFrame(Shape& shape) {

  fillRect(0, 0, 128, 16, BLACK);
  fillRect(shape.xOldPos, shape.yOldPos, SIZE_SHAPE, SIZE_SHAPE, BLACK);
  shape.xOldPos = shape.xPos;
  shape.yOldPos = shape.yPos;

  fillRect(shape.xPos, shape.yPos, SIZE_SHAPE, SIZE_SHAPE, WHITE);

  // dispCord();
  display();
}
