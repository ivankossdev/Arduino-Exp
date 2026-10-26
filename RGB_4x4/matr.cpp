#include <Adafruit_NeoPixel.h>
#include "matr.h"
#include "config.h"

/* == матрица 4x4 ==
 *
 *   0  1  2  3   | row 0   (col 0..3 слева-направо)
 *   7  6  5  4   | row 1   (col 0..3 справа-налево)
 *   8  9 10 11   | row 2   (col 0..3 слева-направо)
 *  15 14 13 12   | row 3   (col 0..3 справа-налево)
 *
 * Перевёрнуты нечётные ряды (1 и 3).
 * =================================================== */

static Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

/* ---------- Координаты пикселя ---------- */

static inline int pixelRow(int pos) {
  return pos / 4;
}

static inline int pixelCol(int pos) {
  int row   = pos / 4;
  int inRow = pos % 4;
  return (row % 2 == 1) ? (3 - inRow) : inRow;
}

static inline int pixelFromRowCol(int row, int col) {
  int inRow = (row % 2 == 1) ? (3 - col) : col;
  return row * 4 + inRow;
}

/* ---------- Направление между двумя соседними пикселями ---------- */

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

const char* directionName(DirectionOfMovement d) {
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

/* ---------- Случайный сосед по 4 направлениям ----------
 * avoid — не выбирать этот пиксель, если есть другие варианты
 * (чтобы точка не дёргалась туда-обратно).
 * ------------------------------------------------------ */
static int pickNeighbor(int pos, int avoid) {
  int r = pixelRow(pos);
  int c = pixelCol(pos);

  int candidates[4];
  int n = 0;

  if (r > 0) { int p = pixelFromRowCol(r - 1, c); if (p != avoid) candidates[n++] = p; }
  if (r < 3) { int p = pixelFromRowCol(r + 1, c); if (p != avoid) candidates[n++] = p; }
  if (c > 0) { int p = pixelFromRowCol(r, c - 1); if (p != avoid) candidates[n++] = p; }
  if (c < 3) { int p = pixelFromRowCol(r, c + 1); if (p != avoid) candidates[n++] = p; }

  if (n == 0) return avoid;   // на 4x4 недостижимо, но пусть будет
  return candidates[random(n)];
}

/* ---------- Публичный API ---------- */

void matrInit(uint8_t br) {
  ring.begin();
  ring.setBrightness(br);
}

DotState dotStep() {
  static int currentPos = -1;   // где точка сейчас (-1 = ещё не было)
  static int prevPos    = -1;   // откуда пришла

  int pos;
  if (currentPos < 0) {
    pos = random(LED_COUNT);            // первый раз — любая клетка
  } else {
    pos = pickNeighbor(currentPos, prevPos);
  }

  ring.clear();
  ring.setPixelColor(pos, ring.Color(0, 0, 255));
  ring.show();

  DotState state;
  state.pos = pos;
  state.row = pixelRow(pos);
  state.col = pixelCol(pos);
  state.dir = getDirection(currentPos, pos);   // при currentPos < 0 → FIRST

  prevPos    = currentPos;
  currentPos = pos;

  return state;
}