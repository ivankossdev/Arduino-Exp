#include "DrawShape.h"

DrawShape::DrawShape(int w, int h) {
  width = w;
  height = h;
  M5.Lcd.setRotation(1);  // Горизонтальная ориентация
  clearScreen();
}

void DrawShape::clearScreen() {
  M5.Lcd.fillScreen(BLACK);
}

void DrawShape::show() {
  // Ничего не нужно — M5.Lcd обновляется мгновенно
}

void DrawShape::drawFrame(const Shape& s) {
  uint16_t color = random(0xFFFF);  // Случайный цвет
  M5.Lcd.fillRect(s.xPos, s.yPos, s.sizeShape, s.sizeShape, color);
}

void DrawShape::dispCord(const Shape& s) {
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("%4d,%4d", s.xPos, s.yPos);
}

void DrawShape::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  M5.Lcd.drawRect(x, y, w, h, color);
}
