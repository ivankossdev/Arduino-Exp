#include "drawShape.h"


DrawShape::DrawShape(int w, int h, TwoWire *wire, int rst) {
  width = w;
  height = h;
  oled = new Adafruit_SSD1306(w, h, wire, rst);
  if (!oled->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.begin(115200);
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // зависаем, если экран не инициализировался
  }
  clearScreen();
}

// Остальные методы остаются
void DrawShape::clearScreen() {
  oled->clearDisplay();
}

void DrawShape::show() {
  oled->display();
}

void DrawShape::drawFrame(const Shape& s) {
  oled->fillRect(s.xPos, s.yPos, s.sizeShape, s.sizeShape, WHITE);
}

void DrawShape::dispCord(const Shape& s) {
  char buf[32];
  snprintf(buf, sizeof(buf), "%d,%d", s.xPos, s.yPos);
  oled->setTextSize(1);
  oled->setCursor(0, 0);
  oled->setTextColor(WHITE);
  oled->println(buf);
}
