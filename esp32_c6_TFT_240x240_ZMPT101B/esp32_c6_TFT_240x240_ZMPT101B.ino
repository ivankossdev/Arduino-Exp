/*
  TFT ST7789 240x240 + ESP32-C6 Super Mini + ZMPT101B
  Визуализация синусоиды сетевого напряжения в реальном времени.

  Подключение:
  TFT VCC  -> 3.3V
  TFT GND  -> GND
  TFT SCL  -> GPIO7 (CLK)
  TFT SDA  -> GPIO6 (MOSI)
  TFT RES  -> GPIO4
  TFT DC   -> GPIO15
  TFT BLK  -> GPIO0
  ZMPT OUT -> GPIO1 (через RC-фильтр: 1кΩ последовательно + 100нФ на GND)
  ZMPT VCC -> 3.3V (с конденсаторами 10мкФ + 100нФ на клеммах)
  ZMPT GND -> GND
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

// --- Пин датчика ZMPT101B ---
#define ZMPT_PIN   1

// --- Параметры графика ---
#define GRAPH_WIDTH        240
#define GRAPH_HEIGHT       180
#define GRAPH_TOP          40
#define SAMPLE_COUNT       120      // 120 точек = 2 периода при 50 Гц
#define SAMPLE_INTERVAL_US 330      // ~3030 Гц частота выборки

// --- Объекты ---
SPIClass* hspi = new SPIClass(SPI);
Adafruit_ST7789 tft = Adafruit_ST7789(hspi, TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16* canvas = nullptr;

// --- Буферы данных ---
int16_t rawData[SAMPLE_COUNT];
int16_t graphData[SAMPLE_COUNT];

// --- Переменные ---
int zeroPoint = 2048;
unsigned long lastDiagnostic = 0;
uint32_t frameCount = 0;

// --- Прототипы ---
void drawHeader();
void drawGraphToCanvas();
void calibrateSensor();
void showDiagnostics(int minV, int maxV, int center, int amplitude);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("===========================================");
  Serial.println(" ST7789 + ZMPT101B: Real-time Sine Wave");
  Serial.println("===========================================");

  // ⚡ ВАЖНО: Настройка АЦП ДО калибровки
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  delay(10);

  // Подсветка TFT (ШИМ, 70% для снижения нагрузки на питание)
  ledcAttach(TFT_BLK, 5000, 8);
  ledcWrite(TFT_BLK, 180);

  // Аппаратный SPI
  hspi->begin(TFT_SCLK, -1, TFT_MOSI, -1);

  // Дисплей
  tft.init(240, 240, SPI_MODE3);
  tft.setSPISpeed(40000000);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  // Canvas (двойная буферизация)
  canvas = new GFXcanvas16(GRAPH_WIDTH, GRAPH_HEIGHT);
  if (canvas == nullptr || canvas->getBuffer() == nullptr) {
    Serial.println("ОШИБКА: не хватило памяти под canvas!");
    while (1) delay(1000);
  }
  Serial.print("Canvas OK: ");
  Serial.print(GRAPH_WIDTH * GRAPH_HEIGHT * 2);
  Serial.println(" байт.");

  // Калибровка нулевой точки
  calibrateSensor();

  // Заголовок
  drawHeader();

  Serial.println("Готово. Начинаем вывод графика...");
}

void loop() {
  // === 1. Сбор выборки ===
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    unsigned long start = micros();
    rawData[i] = analogRead(ZMPT_PIN);
    while (micros() - start < SAMPLE_INTERVAL_US);
  }

  // === 2. Находим min/max ===
  int16_t minV = 4095, maxV = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    if (rawData[i] < minV) minV = rawData[i];
    if (rawData[i] > maxV) maxV = rawData[i];
  }

  int16_t center = (maxV + minV) / 2;
  int16_t amplitude = (maxV - minV) / 2;
  if (amplitude < 5) amplitude = 5;   // защита от деления на 0

  // === 3. Масштабирование в диапазон графика ===
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    float norm = (float)(rawData[i] - center) / amplitude;  // [-1, 1]
    int16_t y = (GRAPH_HEIGHT / 2) + (int16_t)(norm * (GRAPH_HEIGHT / 2 - 5));
    if (y < 0) y = 0;
    if (y > GRAPH_HEIGHT - 1) y = GRAPH_HEIGHT - 1;
    graphData[i] = y;
  }

  // === 4. Рисуем в canvas и отправляем на дисплей ===
  drawGraphToCanvas();
  tft.drawRGBBitmap(0, GRAPH_TOP, canvas->getBuffer(),
                    GRAPH_WIDTH, GRAPH_HEIGHT);

  // === 5. Диагностика раз в секунду ===
  frameCount++;
  if (millis() - lastDiagnostic > 1000) {
    lastDiagnostic = millis();
    showDiagnostics(minV, maxV, center, amplitude);
  }
}

// ============================================================
// Калибровка нулевой точки
// ============================================================
void calibrateSensor() {
  Serial.println("Калибровка нулевой точки...");
  long sum = 0;
  const int N = 500;
  for (int i = 0; i < N; i++) {
    sum += analogRead(ZMPT_PIN);
    delayMicroseconds(200);
  }
  zeroPoint = sum / N;
  Serial.print("Нулевая точка: ");
  Serial.println(zeroPoint);

  int minV = 4095, maxV = 0;
  for (int i = 0; i < 200; i++) {
    int v = analogRead(ZMPT_PIN);
    if (v < minV) minV = v;
    if (v > maxV) maxV = v;
  }
  Serial.print("Во время калибровки: min=");
  Serial.print(minV);
  Serial.print(", max=");
  Serial.println(maxV);
}

// ============================================================
// Заголовок
// ============================================================
void drawHeader() {
  tft.fillRect(0, 0, 240, GRAPH_TOP, ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 5);
  tft.print("AC Voltage");

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(5, 25);
  tft.print("ZMPT101B / 50Hz");
}

// ============================================================
// Отрисовка графика в canvas
// ============================================================
void drawGraphToCanvas() {
  canvas->fillScreen(ST77XX_BLACK);

  // Оси
  canvas->drawFastVLine(0, 0, GRAPH_HEIGHT, ST77XX_WHITE);
  canvas->drawFastHLine(0, GRAPH_HEIGHT - 1, GRAPH_WIDTH, ST77XX_WHITE);

  // Центральная линия (ноль)
  canvas->drawFastHLine(0, GRAPH_HEIGHT / 2, GRAPH_WIDTH, 0x4208);

  // Вертикальная сетка
  for (int x = 60; x < GRAPH_WIDTH; x += 60) {
    for (int y = 0; y < GRAPH_HEIGHT; y += 6) {
      canvas->drawPixel(x, y, 0x4208);
    }
  }

  // Кривая
  for (int x = 0; x < SAMPLE_COUNT - 1; x++) {
    int x1 = (x * GRAPH_WIDTH) / SAMPLE_COUNT;
    int x2 = ((x + 1) * GRAPH_WIDTH) / SAMPLE_COUNT;
    canvas->drawLine(x1, graphData[x], x2, graphData[x + 1], ST77XX_GREEN);
  }
}

// ============================================================
// Диагностика в Serial
// ============================================================
void showDiagnostics(int minV, int maxV, int center, int amplitude) {
  Serial.print("Frame #");
  Serial.print(frameCount);
  Serial.print("  min=");
  Serial.print(minV);
  Serial.print("  max=");
  Serial.print(maxV);
  Serial.print("  center=");
  Serial.print(center);
  Serial.print("  amp=");
  Serial.print(amplitude);

  // Предупреждение о клиппинге
  if (minV < 100) {
    Serial.print("  ⚠ КЛИППИНГ СНИЗУ (уменьши усиление!)");
  } else if (maxV > 4000) {
    Serial.print("  ⚠ КЛИППИНГ СВЕРХУ (уменьши усиление!)");
  } else if (amplitude < 200) {
    Serial.print("  (сигнал слабый — добавь усиления)");
  } else {
    Serial.print("  ✓");
  }
  Serial.println();
}