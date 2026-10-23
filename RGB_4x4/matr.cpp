#include <Adafruit_NeoPixel.h>
#include "matr.h"
#include "config.h"

/* == матрица 4x4 ==
 *
 *  пиксель → (row, col):
 *
 *   0  1  2  3   | row 0   (col 0..3 слева-направо)
 *   7  6  5  4   | row 1   (col 0..3 справа-налево)
 *   8  9 10 11   | row 2   (col 0..3 слева-направо)
 *  15 14 13 12   | row 3   (col 0..3 справа-налево)
 *
 * Перевёрнуты нечётные ряды (1 и 3).
 * =================================================== */

static Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

static const int LED_PATH[] = { 0, 1, 2, 3, 4, 11, 12, 13, 14, 15, 8, 7 };
static const int LED_PATH_LEN = sizeof(LED_PATH) / sizeof(LED_PATH[0]);

enum DirectionOfMovement {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  STANDSTILL,   // точка не сдвинулась: from == to
  JUMP,         // сдвинулась, но не на соседнюю клетку
  FIRST         // предыдущей позиции ещё не было (from < 0)
};

static inline int pixelRow(int pos) {
  return pos / 4;
}

static inline int pixelCol(int pos) {
  int row   = pos / 4;
  int inRow = pos % 4;
  return (row % 2 == 1) ? (3 - inRow) : inRow;
}

static DirectionOfMovement getDirection(int from, int to) {
  if (from < 0)   return FIRST;
  if (from == to) return STANDSTILL;

  int dr = pixelRow(to) - pixelRow(from);
  int dc = pixelCol(to) - pixelCol(from);

  if (dr == 0  && dc ==  1) return RIGHT;
  if (dr == 0  && dc == -1) return LEFT;
  if (dr ==  1 && dc == 0) return DOWN;
  if (dr == -1 && dc == 0) return UP;
  return JUMP;
}

static const char* directionName(DirectionOfMovement d) {
  switch (d) {
    case UP:         return "UP";
    case DOWN:       return "DOWN";
    case LEFT:       return "LEFT";
    case RIGHT:      return "RIGHT";
    case STANDSTILL: return "STANDSTILL";
    case JUMP:       return "JUMP";
    case FIRST:      return "FIRST";
    default:         return "?";
  }
}

void matrInit(uint8_t br) {
  ring.begin();
  ring.setBrightness(br);
}

void dotStep() {
  static int pathIndex = 0;
  static int prevPos   = -1;

  int pos = LED_PATH[pathIndex];

  ring.clear();
  ring.setPixelColor(pos, ring.Color(0, 0, 255));
  ring.show();

  DirectionOfMovement dir = getDirection(prevPos, pos);

  Serial.print("pos ");
  Serial.print(pos);
  Serial.print(" (r");
  Serial.print(pixelRow(pos));
  Serial.print(", c");
  Serial.print(pixelCol(pos));
  Serial.print(") dir ");
  Serial.println(directionName(dir));

  prevPos   = pos;
  pathIndex = (pathIndex + 1) % LED_PATH_LEN;
}