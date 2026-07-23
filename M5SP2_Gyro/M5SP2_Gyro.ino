#include <M5Unified.h>
#include "MovableBox.h"
#include "Wall.h"

int16_t SCREEN_W;
int16_t SCREEN_H;

const uint16_t BG_COLOR = TFT_BLACK;
const uint16_t BOX_OUTLINE = TFT_GREEN;
const uint16_t BOX_FILL = TFT_CYAN;
const uint16_t WALL_COLOR = TFT_RED;

MovableBox box(50, 50, 20, 20);

Wall walls[] = {
  Wall(0, 40, 80, 10),
  Wall(120, 40, 10, 40),
  // Wall(180, 20, 10, 80),
  // Wall(80, 110, 120, 10)
};
const int WALL_COUNT = sizeof(walls) / sizeof(walls[0]);

unsigned long lastPhysicsMs = 0;
unsigned long lastDrawMs = 0;

const unsigned long physicsInterval = 33;  // ~30 FPS для физики
const unsigned long drawInterval = 16;     // ~60 FPS для отрисовки

void setup() 
{
  M5.begin();
  M5.Lcd.setRotation(3);

  if (!M5.Imu.begin()) {
    M5.Lcd.setTextSize(0);
    M5.Lcd.setTextColor(TFT_RED);
    M5.Lcd.fillScreen(BG_COLOR);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Imu not found!");
    while (1)
      ;
  }

  M5.Lcd.fillScreen(BG_COLOR);

  for (int i = 0; i < WALL_COUNT; ++i) {
    walls[i].draw(M5.Lcd, WALL_COLOR);
  }

  box.draw(M5.Lcd, BOX_OUTLINE, BOX_FILL);

  SCREEN_W = M5.Lcd.width();
  SCREEN_H = M5.Lcd.height();

  // Serial.begin(115200);

  delay(500);

}

void loop() {
  unsigned long now = millis();

  // Физика (30 раз в секунду)
  if (now - lastPhysicsMs >= physicsInterval) {
    lastPhysicsMs = now;

    float gx, gy, gz;
    M5.Imu.getGyroData(&gx, &gy, &gz);

    const float GYRO_FACTOR = 0.05f;
    const float DEADZONE = 1.5f;
    const int16_t MAX_SPEED = 5;
    const float FRICTION = 0.99f;
    const float BOUNCE_FACTOR = 0.3f;

    // Считаем ускорение из гироскопа
    float ax = -gx * GYRO_FACTOR; // изменил направление 
    float ay = gy * GYRO_FACTOR;

    if (fabs(ax) < DEADZONE) ax = 0;
    if (fabs(ay) < DEADZONE) ay = 0;

    // Интегрируем ускорение в скорость
    box.vx += ax;
    box.vy += ay;

    // Трение (чтобы не летал вечно)
    box.vx *= FRICTION;
    box.vy *= FRICTION;

    // Ограничение максимальной скорости
    if (box.vx > MAX_SPEED) box.vx = MAX_SPEED;
    if (box.vx < -MAX_SPEED) box.vx = -MAX_SPEED;
    if (box.vy > MAX_SPEED) box.vy = MAX_SPEED;
    if (box.vy < -MAX_SPEED) box.vy = -MAX_SPEED;

    // Serial.printf("Gyro: [ l_gx %.2f, l_gy %.2f ] [box.vx %.2f, box.vy %.2f]\n",
    //               gx, gy, box.vx, box.vy);

    box.update();
    box.bounceEdges(SCREEN_W, SCREEN_H, walls, WALL_COUNT, BOUNCE_FACTOR);
  }

  // Отрисовка
  if (now - lastDrawMs >= drawInterval) {
    lastDrawMs = now;
    box.clear(M5.Lcd, BG_COLOR);
    box.draw(M5.Lcd, BOX_OUTLINE, BOX_FILL);
  }
}
