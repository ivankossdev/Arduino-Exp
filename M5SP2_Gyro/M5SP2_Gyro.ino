#include <M5Unified.h>

const float GYRO_FACTOR = 0.05f;
const int16_t SCREEN_W = 240;
const int16_t SCREEN_H = 135;

class MovableBox {
public:
  int16_t x, y;
  int16_t w, h;
  int16_t vx, vy;

  MovableBox(int16_t _x, int16_t _y, int16_t _w, int16_t _h)
    : x(_x), y(_y), w(_w), h(_h), vx(0), vy(0) {}

  void update() {
    x += vx;
    y += vy;
  }

  void bounceEdges() {
    if (x < 0) {
      x = 0;
      vx = -vx;
      if (vx == 0) vx = 1;
    }
    if (x + w > SCREEN_W) {
      x = SCREEN_W - w;
      vx = -vx;
      if (vx == 0) vx = -1;
    }
    if (y < 0) {
      y = 0;
      vy = -vy;
      if (vy == 0) vy = 1;
    }
    if (y + h > SCREEN_H) {
      y = SCREEN_H - h;
      vy = -vy;
      if (vy == 0) vy = -1;
    }
  }
};

MovableBox box(50, 50, 20, 20);

unsigned long lastPhysicsMs = 0;
const unsigned long physicsInterval = 33;  // ~30 fps для физики

// Отдельный цвет фона, чтобы стирать им
const uint16_t BG_COLOR = TFT_BLACK;
const uint16_t BOX_OUTLINE = TFT_GREEN;
const uint16_t BOX_FILL = TFT_CYAN;

void setup() {
  M5.begin();

  if (!M5.Imu.begin()) {
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_RED);
    M5.Lcd.fillScreen(BG_COLOR);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Imu not found!");
    while (1);
  }

  // Рисуем стартовую позицию один раз
  M5.Lcd.drawRect(box.x, box.y, box.w, box.h, BOX_OUTLINE);
  M5.Lcd.fillRect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, BOX_FILL);

}

void loop() {
  unsigned long now = millis();

  // Физика — строго по таймеру
  if (now - lastPhysicsMs >= physicsInterval) {
    lastPhysicsMs = now;

    float l_gx, l_gy, l_dummy_gz;
    M5.Imu.getGyroData(&l_gx, &l_gy, &l_dummy_gz);

    box.vx = static_cast<int16_t>(l_gx * GYRO_FACTOR);
    box.vy = static_cast<int16_t>(l_gy * GYRO_FACTOR);

    if (box.vx == 0 && abs(l_gx) > 0.5f) box.vx = (l_gx > 0) ? 1 : -1;
    if (box.vy == 0 && abs(l_gy) > 0.5f) box.vy = (l_gy > 0) ? 1 : -1;

    // Сохраняем старые координаты для стирания
    int16_t oldX = box.x;
    int16_t oldY = box.y;

    box.update();
    box.bounceEdges();

    // --- ПЛАВНАЯ ОТРИСОВКА ---

    // 1. Стираем старое положение (рисуем фоном)
    M5.Lcd.drawRect(oldX, oldY, box.w, box.h, BG_COLOR);
    M5.Lcd.fillRect(oldX + 1, oldY + 1, box.w - 2, box.h - 2, BG_COLOR);

    // 2. Рисуем новое положение
    M5.Lcd.drawRect(box.x, box.y, box.w, box.h, BOX_OUTLINE);
    M5.Lcd.fillRect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, BOX_FILL);
  }
}
