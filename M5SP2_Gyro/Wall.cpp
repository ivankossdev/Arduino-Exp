#include "Wall.h"

void Wall::draw(M5GFX &lcd, uint16_t color) const {
  lcd.drawRect(x, y, w, h, color);
  lcd.fillRect(x + 1, y + 1, w - 2, h - 2, color);
}
