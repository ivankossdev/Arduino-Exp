#include "MovableBox.h"

MovableBox::MovableBox(int16_t _x, int16_t _y, int16_t _w, int16_t _h)
  : x(_x), y(_y), w(_w), h(_h), vx(0), vy(0) {
  prevX = x;
  prevY = y;
}

void MovableBox::update() {
  prevX = x;
  prevY = y;

  x += static_cast<int16_t>(vx);
  y += static_cast<int16_t>(vy);
}

static bool rectIntersect(int16_t x1, int16_t y1, int16_t w1, int16_t h1,
                          int16_t x2, int16_t y2, int16_t w2, int16_t h2) {
  return !(x1 + w1 <= x2 || x2 + w2 <= x1 ||
           y1 + h1 <= y2 || y2 + h2 <= y1);
}

void MovableBox::bounceEdges(int16_t screenW, int16_t screenH,
                             const Wall* walls, int wallCount,
                             float bounceFactor) {
  // Отскок от границ экрана
  if (x < 0) {
    x = 0;
    vx = -vx * bounceFactor;
  }
  if (x + w > screenW) {
    x = screenW - w;
    vx = -vx * bounceFactor;
  }
  if (y < 0) {
    y = 0;
    vy = -vy * bounceFactor;
  }
  if (y + h > screenH) {
    y = screenH - h;
    vy = -vy * bounceFactor;
  }

  for (int i = 0; i < wallCount; ++i) {
    const Wall& wall = walls[i];
    if (!rectIntersect(x, y, w, h, wall.x, wall.y, wall.w, wall.h)) {
      continue;
    }

    int16_t prevX_local = x - static_cast<int16_t>(vx);
    int16_t prevY_local = y - static_cast<int16_t>(vy);

    bool hitHoriz = false;
    if (prevX_local + w <= wall.x && x + w > wall.x) hitHoriz = true;
    if (prevX_local >= wall.x + wall.w && x < wall.x + wall.w) hitHoriz = true;

    bool hitVert = false;
    if (prevY_local + h <= wall.y && y + h > wall.y) hitVert = true;
    if (prevY_local >= wall.y + wall.h && y < wall.y + wall.h) hitVert = true;

    // Если попали и по горизонтали, и по вертикали — определяем, какая сторона «первой»
    if (hitHoriz && hitVert) {
      int16_t penX = (x + w > wall.x ? wall.x - (x + w) : (wall.x + wall.w) - x);
      int16_t penY = (y + h > wall.y ? wall.y - (y + h) : (wall.y + wall.h) - y);
      if (abs(penX) < abs(penY)) {
        hitVert = false;
      } else {
        hitHoriz = false;
      }
    }

    if (hitHoriz) {
      vx = -vx * bounceFactor;
      if (x < wall.x) {
        x = wall.x - w;
      } else {
        x = wall.x + wall.w;
      }
    }

    if (hitVert) {
      vy = -vy * bounceFactor;
      if (y < wall.y) {
        y = wall.y - h;
      } else {
        y = wall.y + wall.h;
      }
    }
  }
}

void MovableBox::clear(M5GFX &lcd, uint16_t bgColor) {
  lcd.drawRect(prevX, prevY, w, h, bgColor);
  lcd.fillRect(prevX + 1, prevY + 1, w - 2, h - 2, bgColor);
}

void MovableBox::draw(M5GFX &lcd, uint16_t outlineColor, uint16_t fillColor) {
  lcd.drawRect(x, y, w, h, outlineColor);
  lcd.fillRect(x + 1, y + 1, w - 2, h - 2, fillColor);
}
