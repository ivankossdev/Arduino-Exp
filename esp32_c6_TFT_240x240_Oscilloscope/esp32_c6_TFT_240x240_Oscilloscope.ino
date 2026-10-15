/*
  Oscilloscope-style display для ESP32-C6 + ST7789 240x240
  Работает и с реальным сигналом (ZMPT101B), и с "плавающим" входом.

  Особенности:
  - Триггер по переднему фронту через DC-центр
  - Гистерезис в измерении частоты
  - Медианный фильтр частоты
  - Детектор отсутствия сигнала (только в заголовке и Serial)
  - centerAvg обновляется только по валидным кадрам
  - Двойная буферизация через GFXcanvas16
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

// --- Датчик / вход ---
#define ZMPT_PIN   1

// --- Разметка ---
#define GRAPH_WIDTH   240
#define GRAPH_HEIGHT  180
#define GRAPH_TOP     40

// --- Захват ---
#define SAMPLE_INTERVAL_US 200       // 5 кГц
#define SAMPLE_COUNT       240       // 48 мс окно

// --- Сетка ---
#define GRID_DIV_X  10
#define GRID_DIV_Y  6

// --- Пороги детектора сигнала ---
#define MIN_VPP_FOR_SIGNAL   250
#define CLIP_LOW_THRESHOLD   30
#define CLIP_HIGH_THRESHOLD  4065

SPIClass* hspi = new SPIClass(SPI);
Adafruit_ST7789 tft = Adafruit_ST7789(hspi, TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16* canvas = nullptr;

int16_t rawData[SAMPLE_COUNT];
int16_t graphData[SAMPLE_COUNT];

float centerAvg = 2048.0;
unsigned long lastStats = 0;
uint32_t frameCount = 0;

// --- Медианный фильтр частоты (окно 5) ---
float freqHistory[5] = {0, 0, 0, 0, 0};
int   freqIdx = 0;

// --- Состояние сигнала ---
bool  signalPresent = false;
bool  prevSignalPresent = false;

// --- Прототипы ---
void  drawHeader();
void  updateHeader(int vpp, float freq, bool hasSignal);
void  drawOscilloscope();
bool  waitForTrigger(unsigned long timeout_ms);
float measureFrequency(int16_t* data, int n, float center);
float filterFreq(float newFreq);

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

  // Первичная оценка DC-центра
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
  // 1. Ожидание триггера
  waitForTrigger(150);

  // 2. Захват выборки
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    unsigned long t = micros();
    rawData[i] = analogRead(ZMPT_PIN);
    while (micros() - t < SAMPLE_INTERVAL_US);
  }

  // 3. min/max текущего кадра
  int16_t minV = 4095, maxV = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    if (rawData[i] < minV) minV = rawData[i];
    if (rawData[i] > maxV) maxV = rawData[i];
  }
  int center = (minV + maxV) / 2;
  int amp    = (maxV - minV) / 2;
  if (amp < 5) amp = 5;
  int vpp = maxV - minV;

  // 4. Определение наличия сигнала
  bool clippedLow  = (minV < CLIP_LOW_THRESHOLD);
  bool clippedHigh = (maxV > CLIP_HIGH_THRESHOLD);
  bool clippedBoth = clippedLow && clippedHigh;
  bool tooSmall    = (vpp < MIN_VPP_FOR_SIGNAL);
  bool lowAnchored = (minV <= 1);

  signalPresent = !tooSmall && !lowAnchored && !clippedBoth;

  // 5. Обновляем DC-центр только по валидным кадрам
  if (signalPresent) {
    float frameMid = (minV + maxV) / 2.0;
    centerAvg = centerAvg * 0.9 + frameMid * 0.1;
  }

  // 6. Масштабирование в пиксели
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    float norm = (float)(rawData[i] - center) / amp;
    int y = (GRAPH_HEIGHT / 2) - (int)(norm * (GRAPH_HEIGHT / 2 - 3));
    if (y < 0) y = 0;
    if (y > GRAPH_HEIGHT - 1) y = GRAPH_HEIGHT - 1;
    graphData[i] = y;
  }

  // 7. Отрисовка + отправка
  drawOscilloscope();

  // 8. Статистика раз в секунду
  frameCount++;
  if (millis() - lastStats > 1000) {
    lastStats = millis();

    float freqRaw = 0;
    if (signalPresent) {
      freqRaw = measureFrequency(rawData, SAMPLE_COUNT, center);
    }
    float freq = filterFreq(freqRaw);

    updateHeader(vpp, freq, signalPresent);

    Serial.print("Frame ");    Serial.print(frameCount);
    Serial.print("  min=");    Serial.print(minV);
    Serial.print("  max=");    Serial.print(maxV);
    Serial.print("  Vpp=");    Serial.print(vpp);
    Serial.print("  F=");      Serial.print(freq, 1);
    Serial.print(" Hz  ");
    Serial.println(signalPresent ? "SIG" : "NO-SIGNAL");
  }

  // Сообщение о смене состояния сигнала
  if (signalPresent != prevSignalPresent) {
    Serial.println(signalPresent ? ">> Signal detected"
                                 : ">> Signal lost");
    prevSignalPresent = signalPresent;
  }
}

// ============================================================
// Триггер: передний фронт через DC-центр с гистерезисом
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
  return false;
}

// ============================================================
// Измерение частоты с гистерезисом
// ============================================================
float measureFrequency(int16_t* data, int n, float center) {
  const int hyst = 40;
  bool below = false;
  int firstRise = -1, lastRise = -1, count = 0;

  for (int i = 0; i < n; i++) {
    if (data[i] < center - hyst) below = true;
    if (below && data[i] > center + hyst) {
      if (firstRise < 0) firstRise = i;
      lastRise = i;
      count++;
      below = false;
    }
  }
  if (count < 2) return 0;

  float dt = (lastRise - firstRise) * (SAMPLE_INTERVAL_US / 1000000.0);
  return (count - 1) / dt;
}

// ============================================================
// Медианный фильтр частоты (окно 5)
// ============================================================
float filterFreq(float newFreq) {
  if (newFreq < 20.0 || newFreq > 200.0) {
    float s[5];
    memcpy(s, freqHistory, sizeof(s));
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4 - i; j++)
        if (s[j] > s[j + 1]) { float t = s[j]; s[j] = s[j + 1]; s[j + 1] = t; }
    return s[2];
  }

  freqHistory[freqIdx] = newFreq;
  freqIdx = (freqIdx + 1) % 5;

  float s[5];
  memcpy(s, freqHistory, sizeof(s));
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4 - i; j++)
      if (s[j] > s[j + 1]) { float t = s[j]; s[j] = s[j + 1]; s[j + 1] = t; }
  return s[2];
}

// ============================================================
// Отрисовка осциллограммы
// ============================================================
void drawOscilloscope() {
  canvas->fillScreen(ST77XX_BLACK);

  // Пунктирная сетка
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

  // Центральные оси
  int midY = GRAPH_HEIGHT / 2;
  int midX = GRAPH_WIDTH / 2;
  for (int x = 0; x < GRAPH_WIDTH; x += 2)
    canvas->drawPixel(x, midY, 0x4208);
  for (int y = 0; y < GRAPH_HEIGHT; y += 2)
    canvas->drawPixel(midX, y, 0x4208);

  // Рамка
  canvas->drawRect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT, ST77XX_WHITE);

  // Луч (кривая)
  for (int x = 0; x < SAMPLE_COUNT - 1; x++) {
    int x1 = (x * GRAPH_WIDTH) / SAMPLE_COUNT;
    int x2 = ((x + 1) * GRAPH_WIDTH) / SAMPLE_COUNT;
    canvas->drawLine(x1, graphData[x], x2, graphData[x + 1], ST77XX_GREEN);
  }

  // Метка уровня триггера
  canvas->fillTriangle(0, midY - 5, 0, midY + 5, 7, midY, ST77XX_YELLOW);

  // Отправка кадра
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
}

void updateHeader(int vpp, float freq, bool hasSignal) {
  tft.fillRect(0, 22, 240, 16, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(5, 24);

  if (!hasSignal) {
    tft.setTextColor(ST77XX_RED);
    tft.print("--- no signal ---");
    return;
  }

  float vppVolts = vpp / 4095.0 * 3.3;

  tft.setTextColor(ST77XX_CYAN);
  tft.print("Vpp=");
  tft.print(vppVolts, 2);
  tft.print("V  ");
  tft.print(freq, 1);
  tft.print("Hz");
}