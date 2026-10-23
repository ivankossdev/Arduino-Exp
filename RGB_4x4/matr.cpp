#include <Adafruit_NeoPixel.h>
#include "matr.h"
#include "config.h"

/* == матрица 4x4 ==
 *
 *  пиксель → (row, col):
 *
 *   0  1  2  3   | row 0   (col 0..3 слева-направо)
 *   4  5  6  7   | row 1   (col 0..3 слева-направо)
 *  11 10  9  8   | row 2   (col 0..3 справа-налево!)
 *  12 13 14 15   | row 3   (col 0..3 слева-направо)
 *
 * =================================================== */

static Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

/* Демонстрационный путь обхода (12 точек из 16). */
static const int LED_PATH[] = { 0, 1, 2, 3, 4, 11, 12, 13, 14, 15, 8, 7 };
static const int LED_PATH_LEN = sizeof(LED_PATH) / sizeof(LED_PATH[0]);

enum DirectionOfMovement {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  STOP
};

/* ---------- Координаты пикселя ---------- */

static inline int pixelRow(int pos) {
  return pos / 4;               // 0..3
}

static inline int pixelCol(int pos) {
  int row   = pos / 4;
  int inRow = pos % 4;
  return (row == 2) ? (3 - inRow) : inRow;   // 0..3
}

/* ---------- Направление между двумя соседними пикселями ----------
 * Для несоседних точек (диагональ, прыжок через ряд) вернёт STOP.
 * --------------------------------------------------------------- */
static DirectionOfMovement getDirection(int from, int to) {
  int dr = pixelRow(to) - pixelRow(from);
  int dc = pixelCol(to) - pixelCol(from);

  if (dr == 0  && dc ==  1) return RIGHT;
  if (dr == 0  && dc == -1) return LEFT;
  if (dr ==  1 && dc == 0) return DOWN;
  if (dr == -1 && dc == 0) return UP;
  return STOP;
}

static const char* directionName(DirectionOfMovement d) {
  switch (d) {
    case UP:    return "UP";
    case DOWN:  return "DOWN";
    case LEFT:  return "LEFT";
    case RIGHT: return "RIGHT";
    default:    return "STOP";
  }
}

/* ---------- Публичный API ---------- */

void matrInit(uint8_t br) {
  ring.begin();
  ring.setBrightness(br);
}

void dotWalk() {
  int prev = -1;   // предыдущая точка — локальная, не глобальная

  for (int i = 0; i < LED_PATH_LEN; i++) {
    int pos = LED_PATH[i];

    ring.clear();
    ring.setPixelColor(pos, ring.Color(0, 0, 255));
    ring.show();

    DirectionOfMovement dir =
        (prev < 0) ? STOP : getDirection(prev, pos);

    Serial.print("pos ");
    Serial.print(pos);
    Serial.print(" (r");
    Serial.print(pixelRow(pos));
    Serial.print(", c");
    Serial.print(pixelCol(pos));
    Serial.print(") dir ");
    Serial.println(directionName(dir));

    prev = pos;
    delay(1000);
  }
}