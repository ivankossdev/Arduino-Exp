/*
  Oscilloscope-style display для ZMPT101B на ST7789 240x240

  Особенности:
  - Триггер по фронту через DC-центр — картинка стабильна
  - Автомасштаб по вертикали
  - Сетка 10x6 делений, центральные оси
  - Метка уровня триггера слева
  - Заголовок: Vpp и развёртка (ms/div)
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// --- Пины TFT ---
#define TFT_MOSI   6
#define TFT_SCLK   7
#define TFT_DC     15
#define TFT_RST    4
#define TFT_BLK    0
#define TFT_CS     -1

// --- Датчик ---
#define ZMPT_PIN   1

// --- Разметка ---
#define GRAPH_WIDTH   240
#define GRAPH_HEIGHT  180
#define GRAPH_TOP     40

// --- Захват ---
#define SAMPLE_INTERVAL_US 200          // 5 kHz
#define SAMPLE_COUNT       240          // 48 мс = ~2.4 периода при 50 Гц

// --- Сетка ---
#define GRID_DIV_X  10
#define GRID_DIV_Y  6

SPIClass* hspi = new SPIClass(SPI);
Adafruit_ST7789 tft = Adafruit_ST7789(hspi, TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16* canvas = nullptr;

int16_t rawData[SAMPLE_COUNT];
int16_t graphData[SAMPLE_COUNT];

float centerAvg = 2048.0;      // медленно следящий DC-центр
unsigned long lastStats = 0;
uint32_t frameCount = 0;

// --- Прототипы ---
void drawHeader();
void updateHeader(int vpp, float freq);
void drawOscilloscope(int vpp);
bool waitForTrigger(unsigned long timeout_ms);
float measureFrequency(int16_t* data, int n, float center);

// ============================================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Oscilloscope mode ===");

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  delay(10);

  ledcAttach(TFT_BLK, 5000, 8);
  ledcWrite(TFT_BLK, 180);

  hspi->begin(TFT_SCLK, -1, TFT_MOSI, -1);

  tft.init(240, 240, SPI_MODE3);
  tft.setSPISpeed(40000000);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  canvas = new GFXcanvas16(GRAPH_WIDTH, GRAPH_HEIGHT);
  if (!canvas || !canvas->getBuffer()) {
    Serial.println("Canvas alloc failed!");
    while (1) delay(1000);
  }
  Serial.println("Canvas OK");

  // Калибровка DC-центра
  long sum = 0;
  for (int i = 0; i < 500; i++) {
    sum += analogRead(ZMPT_PIN);
    delayMicroseconds(200);
  }
  centerAvg = sum / 500.0;
  Serial.print("Center: "); Serial.println(centerAvg);

  drawHeader();
  Serial.println("Running...");
}

// ============================================================
void loop() {
  // 1. Ждём триггер (передний фронт через центр)
  waitForTrigger(150);

  // 2. Захват выборки
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    unsigned long t = micros();
    rawData[i] = analogRead(ZMPT_PIN);
    while (micros() - t < SAMPLE_INTERVAL_US);
  }

  // 3. Медленно обновляем DC-центр
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    centerAvg = centerAvg * 0.995 + rawData[i] * 0.005;
  }

  // 4. Мин/макс текущего кадра
  int16_t minV = 4095, maxV = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    if (rawData[i] < minV) minV = rawData[i];
    if (rawData[i] > maxV) maxV = rawData[i];
  }
  int center = (minV + maxV) / 2;
  int amp = (maxV - minV) / 2;
  if (amp < 5) amp = 5;
  int vpp = maxV - minV;

  // 5. Масштабируем в пиксели
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    float norm = (float)(rawData[i] - center) / amp;   // [-1, 1]
    int y = (GRAPH_HEIGHT / 2) - (int)(norm * (GRAPH_HEIGHT / 2 - 3));
    if (y < 0) y = 0;
    if (y > GRAPH_HEIGHT - 1) y = GRAPH_HEIGHT - 1;
    graphData[i] = y;
  }

  // 6. Рисуем и отправляем
  drawOscilloscope(vpp);

  // 7. Статистика раз в секунду
  frameCount++;
  if (millis() - lastStats > 1000) {
    lastStats = millis();
    float freq = measureFrequency(rawData, SAMPLE_COUNT, center);
    updateHeader(vpp, freq);
    Serial.print("Frame "); Serial.print(frameCount);
    Serial.print("  min="); Serial.print(minV);
    Serial.print("  max="); Serial.print(maxV);
    Serial.print("  Vpp(ADC)="); Serial.print(vpp);
    Serial.print("  F="); Serial.print(freq, 1);
    Serial.println(" Hz");
  }
}

// ============================================================
// Ожидание триггера: передний фронт через центр
// ============================================================
bool waitForTrigger(unsigned long timeout_ms) {
  unsigned long start = millis();
  const int hyst = 25;
  bool armed = false;

  while (millis() - start < timeout_ms) {
    int curr = analogRead(ZMPT_PIN);
    if (curr < centerAvg - hyst) armed = true;
    if (armed && curr >= centerAvg) return true;
  }
  return false;   // таймаут — захватим без синхронизации
}

// ============================================================
// Частота: считаем передние фронты
// ============================================================
float measureFrequency(int16_t* data, int n, float center) {
  int firstCross = -1, lastCross = -1, crossings = 0;
  for (int i = 1; i < n; i++) {
    if (data[i - 1] < center && data[i] >= center) {
      if (firstCross < 0) firstCross = i;
      lastCross = i;
      crossings++;
    }
  }
  if (crossings < 2) return 0;

  float dt = (lastCross - firstCross) * (SAMPLE_INTERVAL_US / 1000000.0);
  float periods = crossings - 1;
  return periods / dt;
}

// ============================================================
// Отрисовка осциллограммы в canvas
// ============================================================
void drawOscilloscope(int vpp) {
  canvas->fillScreen(ST77XX_BLACK);

  // --- Пунктирная сетка ---
  for (int i = 1; i < GRID_DIV_X; i++) {
    int x = (i * GRAPH_WIDTH) / GRID_DIV_X;
    for (int y = 0; y < GRAPH_HEIGHT; y += 4)
      canvas->drawPixel(x, y, 0x2104);
  }
  for (int i = 1; i < GRID_DIV_Y; i++) {
    int y = (i * GRAPH_HEIGHT) / GRID_DIV_Y;
    for (int x = 0; x < GRAPH_WIDTH; x += 4)
      canvas->drawPixel(x, y, 0x2104);
  }

  // --- Центральные оси (ярче) ---
  int midY = GRAPH_HEIGHT / 2;
  int midX = GRAPH_WIDTH / 2;
  for (int x = 0; x < GRAPH_WIDTH; x += 2)
    canvas->drawPixel(x, midY, 0x4208);
  for (int y = 0; y < GRAPH_HEIGHT; y += 2)
    canvas->drawPixel(midX, y, 0x4208);

  // --- Рамка ---
  canvas->drawRect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT, ST77XX_WHITE);

  // --- Луч (кривая) ---
  for (int x = 0; x < SAMPLE_COUNT - 1; x++) {
    int x1 = (x * GRAPH_WIDTH) / SAMPLE_COUNT;
    int x2 = ((x + 1) * GRAPH_WIDTH) / SAMPLE_COUNT;
    canvas->drawLine(x1, graphData[x], x2, graphData[x + 1], ST77XX_GREEN);
  }

  // --- Метка уровня триггера слева ---
  canvas->fillTriangle(0, midY - 5, 0, midY + 5, 7, midY, ST77XX_YELLOW);

  // --- Отправка на дисплей ---
  tft.drawRGBBitmap(0, GRAPH_TOP, canvas->getBuffer(),
                    GRAPH_WIDTH, GRAPH_HEIGHT);
}

// ============================================================
// Заголовок
// ============================================================
void drawHeader() {
  tft.fillRect(0, 0, 240, GRAPH_TOP, ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 2);
  tft.print("OSC AUTO");

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(5, 24);
  tft.print("Vpp=--.--V  --.-Hz");
}

void updateHeader(int vpp, float freq) {
  // Переводим ADC-отсчёты в вольты (грубо: 3.3 В / 4095)
  float vppVolts = vpp / 4095.0 * 3.3;

  tft.fillRect(0, 22, 240, 16, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(5, 24);
  tft.print("Vpp=");
  tft.print(vppVolts, 2);
  tft.print("V  ");
  tft.print(freq, 1);
  tft.print("Hz");
}