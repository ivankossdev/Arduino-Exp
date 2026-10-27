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

/* ---------- Состояние ---------- */

static int g_currentPos = -1;
static int g_prevPos    = -1;
static int g_targetPos  = -1;

static bool          g_flashing       = false;   // STANDSTILL
static unsigned long g_flashStart     = 0;

static bool          g_goalFlashing   = false;   // достижение цели
static unsigned long g_goalFlashStart = 0;

/* ---------- Цвета ---------- */

static uint32_t colorDot()    { return ring.Color(COLOR_DOT_R,    COLOR_DOT_G,    COLOR_DOT_B); }
static uint32_t colorFlash()  { return ring.Color(COLOR_FLASH_R,  COLOR_FLASH_G,  COLOR_FLASH_B); }
static uint32_t colorTarget() { return ring.Color(COLOR_TARGET_R, COLOR_TARGET_G, COLOR_TARGET_B); }
static uint32_t colorGoal()   { return ring.Color(COLOR_GOAL_R,   COLOR_GOAL_G,   COLOR_GOAL_B); }

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

/* ---------- Отрисовка кадра ----------
 * Рисует цель (если не совпадает с точкой) и точку.
 * Цвет точки зависит от состояния вспышек. */
static void renderFrame() {
  ring.clear();

  if (g_targetPos >= 0 && g_targetPos != g_currentPos) {
    ring.setPixelColor(g_targetPos, colorTarget());
  }

  if (g_currentPos >= 0) {
    uint32_t c;
    if      (g_goalFlashing) c = colorGoal();    // приоритет у успеха
    else if (g_flashing)     c = colorFlash();
    else                     c = colorDot();
    ring.setPixelColor(g_currentPos, c);
  }

  ring.show();
}

/* ---------- Выбор новой цели ---------- */

static int pickTarget(int exclude) {
  if (LED_COUNT <= 1) return exclude;
  int t;
  do {
    t = random(LED_COUNT);
  } while (t == exclude);
  return t;
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
  g_currentPos     = startPos;
  g_prevPos        = -1;
  g_flashing       = false;
  g_goalFlashing   = false;
  g_targetPos      = pickTarget(startPos);
  renderFrame();

  DotState s;
  s.pos           = startPos;
  s.row           = pixelRow(startPos);
  s.col           = pixelCol(startPos);
  s.dir           = FIRST;
  s.reachedTarget = false;
  return s;
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
  g_flashing   = false;

  DotState s;
  s.pos           = g_currentPos;
  s.row           = pixelRow(g_currentPos);
  s.col           = pixelCol(g_currentPos);
  s.dir           = dir;
  s.reachedTarget = false;

  if (g_currentPos == g_targetPos) {
    s.reachedTarget   = true;
    g_goalFlashing    = true;
    g_goalFlashStart  = millis();
    g_targetPos       = pickTarget(g_currentPos);
  }

  renderFrame();
  return s;
}

DotState dotMoveDirection(DirectionOfMovement dir) {
  DotState s;
  s.reachedTarget = false;

  if (g_currentPos < 0) {
    s.pos = -1; s.row = -1; s.col = -1;
    s.dir = FIRST;
    return s;
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
      s.pos = g_currentPos; s.row = r; s.col = c;
      s.dir = STANDSTILL;
      return s;
  }

  // Упор в край — вспышка, точка стоит
  if (nr < 0 || nr > 3 || nc < 0 || nc > 3) {
    g_flashing   = true;
    g_flashStart = millis();
    renderFrame();
    s.pos = g_currentPos; s.row = r; s.col = c;
    s.dir = STANDSTILL;
    return s;
  }

  int newPos = pixelFromRowCol(nr, nc);

  g_prevPos    = g_currentPos;
  g_currentPos = newPos;
  g_flashing   = false;

  s.pos = g_currentPos;
  s.row = pixelRow(g_currentPos);
  s.col = pixelCol(g_currentPos);
  s.dir = dir;

  if (g_currentPos == g_targetPos) {
    s.reachedTarget  = true;
    g_goalFlashing   = true;
    g_goalFlashStart = millis();
    g_targetPos      = pickTarget(g_currentPos);
  }

  renderFrame();
  return s;
}

void dotVisualTick() {
  bool needRender = false;

  if (g_flashing && (millis() - g_flashStart) >= STANDSTILL_FLASH_MS) {
    g_flashing = false;
    needRender = true;
  }
  if (g_goalFlashing && (millis() - g_goalFlashStart) >= GOAL_FLASH_MS) {
    g_goalFlashing = false;
    needRender = true;
  }

  if (needRender) renderFrame();
}