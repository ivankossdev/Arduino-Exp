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

/* ---------- Состояние точки ---------- */

static int g_currentPos = -1;
static int g_prevPos    = -1;

/* ---------- Состояние вспышки ---------- */

static bool          g_flashing   = false;
static unsigned long g_flashStart = 0;

/* ---------- Цвета ---------- */

static uint32_t colorDot()   { return ring.Color(COLOR_DOT_R,   COLOR_DOT_G,   COLOR_DOT_B); }
static uint32_t colorFlash() { return ring.Color(COLOR_FLASH_R, COLOR_FLASH_G, COLOR_FLASH_B); }

/* ---------- Координаты пикселя ---------- */

static inline int pixelRow(int pos) { return pos / 4; }

static inline int pixelCol(int pos) {
  int row   = pos / 4;
  int inRow = pos % 4;
  return (row % 2 == 1) ? (3 - inRow) : inRow;
}

static inline int pixelFromRowCol(int row, int col) {
  int inRow = (row % 2 == 1) ? (3 - col) : col;
  return row * 4 + inRow;
}

/* ---------- Направление между двумя пикселями ---------- */

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

/* ---------- Служебное: отрисовать пиксель нужным цветом ---------- */

static DotState drawAt(int pos, DirectionOfMovement dir, uint32_t color) {
  ring.clear();
  if (pos >= 0) ring.setPixelColor(pos, color);
  ring.show();

  DotState s;
  s.pos = pos;
  s.row = (pos >= 0) ? pixelRow(pos) : -1;
  s.col = (pos >= 0) ? pixelCol(pos) : -1;
  s.dir = dir;
  return s;
}

/* ---------- Случайный сосед (для dotStep) ---------- */

static int pickNeighbor(int pos, int avoid) {
  int r = pixelRow(pos);
  int c = pixelCol(pos);

  int candidates[4];
  int n = 0;

  if (r > 0) { int p = pixelFromRowCol(r - 1, c); if (p != avoid) candidates[n++] = p; }
  if (r < 3) { int p = pixelFromRowCol(r + 1, c); if (p != avoid) candidates[n++] = p; }
  if (c > 0) { int p = pixelFromRowCol(r, c - 1); if (p != avoid) candidates[n++] = p; }
  if (c < 3) { int p = pixelFromRowCol(r, c + 1); if (p != avoid) candidates[n++] = p; }

  if (n == 0) return avoid;
  return candidates[random(n)];
}

/* ---------- Публичный API ---------- */

void matrInit(uint8_t br) {
  ring.begin();
  ring.setBrightness(br);
}

DotState dotInit(int startPos) {
  g_currentPos = startPos;
  g_prevPos    = -1;
  g_flashing   = false;
  return drawAt(startPos, FIRST, colorDot());
}

DotState dotStep() {
  int pos;
  if (g_currentPos < 0) {
    pos = random(LED_COUNT);
  } else {
    pos = pickNeighbor(g_currentPos, g_prevPos);
  }

  DirectionOfMovement dir = getDirection(g_currentPos, pos);

  g_prevPos    = g_currentPos;
  g_currentPos = pos;
  g_flashing   = false;   // авто-шаг отменяет любую незавершённую вспышку

  return drawAt(pos, dir, colorDot());
}

DotState dotMoveDirection(DirectionOfMovement dir) {
  if (g_currentPos < 0) {
    return drawAt(-1, FIRST, colorDot());
  }

  int r = pixelRow(g_currentPos);
  int c = pixelCol(g_currentPos);

  int nr = r, nc = c;
  switch (dir) {
    case UP:    nr = r - 1; break;
    case DOWN:  nr = r + 1; break;
    case LEFT:  nc = c - 1; break;
    case RIGHT: nc = c + 1; break;
    default:
      return drawAt(g_currentPos, STANDSTILL, colorDot());
  }

  // Упор в край — вспышка
  if (nr < 0 || nr > 3 || nc < 0 || nc > 3) {
    g_flashing   = true;
    g_flashStart = millis();
    return drawAt(g_currentPos, STANDSTILL, colorFlash());
  }

  int newPos = pixelFromRowCol(nr, nc);

  g_prevPos    = g_currentPos;
  g_currentPos = newPos;
  g_flashing   = false;   // движение отменяет вспышку

  return drawAt(newPos, dir, colorDot());
}

void dotVisualTick() {
  if (!g_flashing) return;
  if (millis() - g_flashStart < STANDSTILL_FLASH_MS) return;

  g_flashing = false;
  if (g_currentPos >= 0) {
    drawAt(g_currentPos, STANDSTILL, colorDot());
  }
}