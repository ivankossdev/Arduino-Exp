#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    16
#define MATRIX_W    4
#define MATRIX_H    4
#define FP          16   // fixed-point множитель (субпиксели)

CRGB leds[NUM_LEDS];

// --- Состояние мухи в субпикселях: 0 .. (MATRIX_W-1)*FP ---
int16_t posX = 1 * FP;
int16_t posY = 1 * FP;
int8_t  velX = 0;   // в субпикселях за кадр
int8_t  velY = 0;

uint16_t frameCounter = 0;

// --- Настройки поведения ---
const int8_t  MAX_VEL      = 4;    // 4/16 = 0.25 px/кадр
const uint8_t WANDER_EVERY = 10;   // менять направление раз в N кадров
const int16_t EDGE_FP      = 10;   // ≈ 0.6 px от края начинаем отталкиваться
const uint8_t TRAIL_FADE   = 35;   // 0 = без шлейфа, 40-60 = лёгкий след

// --- Маппинг: поменяйте, если матрица распаяна "змейкой" ---
uint16_t XY(uint8_t x, uint8_t y) {
  // if (y & 1) return y * MATRIX_W + (MATRIX_W - 1 - x); // serpentine
  return y * MATRIX_W + x;
}

// Гамма-таблица (sqrt-подобная): weight 0..255 -> яркость 0..255
// Делает тусклые углы bilinear-квада заметнее
static const uint8_t GAMMA_LUT[16] = {
  0, 64, 90, 111, 128, 143, 157, 169,
  180, 191, 202, 212, 221, 230, 238, 246
};
inline uint8_t gamma(uint8_t w) { return GAMMA_LUT[w >> 4]; }

// Добавляет свет к пикселю (max, чтобы шлейф не "пересвечивался")
void addLight(int8_t x, int8_t y, uint8_t b) {
  if (x < 0 || x >= MATRIX_W || y < 0 || y >= MATRIX_H || b == 0) return;
  uint16_t i = XY(x, y);
  uint8_t cur = max(max(leds[i].r, leds[i].g), leds[i].b);
  uint8_t nv  = (cur > b) ? cur : b;
  leds[i] = CRGB(nv, nv, nv);
}

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(120);
  randomSeed(analogRead(0));
}

void loop() {
  // 1. Случайное блуждание
  if (frameCounter % WANDER_EVERY == 0) {
    velX += random(-1, 2);
    velY += random(-1, 2);
  }

  // 2. Избегание стен (краёв сетки)
  if (posX < EDGE_FP)                          velX += 1;
  if (posX > (MATRIX_W - 1) * FP - EDGE_FP)    velX -= 1;
  if (posY < EDGE_FP)                          velY += 1;
  if (posY > (MATRIX_H - 1) * FP - EDGE_FP)    velY -= 1;

  velX = constrain(velX, -MAX_VEL, MAX_VEL);
  velY = constrain(velY, -MAX_VEL, MAX_VEL);

  // 3. Движение
  posX += velX;
  posY += velY;

  // 4. Жёсткая коллизия
  int16_t maxX = (MATRIX_W - 1) * FP;
  int16_t maxY = (MATRIX_H - 1) * FP;
  if (posX < 0)    { posX = 0;    velX = -velX; }
  if (posX > maxX) { posX = maxX; velX = -velX; }
  if (posY < 0)    { posY = 0;    velY = -velY; }
  if (posY > maxY) { posY = maxY; velY = -velY; }

  // 5. Отрисовка (шлейф + bilinear)
  if (TRAIL_FADE > 0) fadeToBlackBy(leds, NUM_LEDS, TRAIL_FADE);
  else                FastLED.clear();

  int8_t  x0 = posX / FP;
  int8_t  y0 = posY / FP;
  int8_t  x1 = x0 + 1;
  int8_t  y1 = y0 + 1;
  uint8_t fx = (posX % FP) * 255 / FP;  // 0..255
  uint8_t fy = (posY % FP) * 255 / FP;

  addLight(x0, y0, gamma(((255 - fx) * (255 - fy)) >> 8));
  addLight(x1, y0, gamma((fx         * (255 - fy)) >> 8));
  addLight(x0, y1, gamma(((255 - fx) * fy        ) >> 8));
  addLight(x1, y1, gamma((fx         * fy        ) >> 8));

  FastLED.show();
  frameCounter++;
  delay(40);   // ~25 FPS
}