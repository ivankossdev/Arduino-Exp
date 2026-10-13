/*
  TFT ST7789 240x240 + ESP32-C6 + ZMPT101B
  Визуализация синусоиды сетевого напряжения.
  ОСНОВНАЯ ЛОГИКА: рисуем "окно" длиной 2 периода (40 мс) для наглядности.
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <ZMPT101B.h>

// --- Пины TFT ---
#define TFT_MOSI   6
#define TFT_SCLK   7
#define TFT_DC     15
#define TFT_RST    4
#define TFT_BLK    0
#define TFT_CS     -1

// --- Пин датчика ---
#define ZMPT_PIN   1   // Подключи OUT датчика к GPIO1

// --- Параметры графика ---
#define GRAPH_WIDTH   240
#define GRAPH_HEIGHT  180
#define GRAPH_TOP     40
#define SAMPLE_COUNT  120  // Количество точек в буфере (для ~2 периодов при 50Гц)

// --- Объекты ---
SPIClass* hspi = new SPIClass(SPI);
Adafruit_ST7789 tft = Adafruit_ST7789(hspi, TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16* canvas = nullptr;
ZMPT101B voltageSensor(ZMPT_PIN);

// --- Буферы данных ---
int16_t rawData[SAMPLE_COUNT];   // Сырые значения АЦП
int16_t graphData[SAMPLE_COUNT]; // Значения для отрисовки (0..GRAPH_HEIGHT-1)

// --- Переменные ---
int zeroPoint = 0;              // Нулевая точка (DC-смещение)
unsigned long lastSample = 0;
const int SAMPLE_INTERVAL_US = 330; // ~330 мкс между отсчетами (для ~3 кГц)

// --- Прототипы ---
void drawHeader();
void drawGraphToCanvas();
void calibrateSensor();

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== ST7789 + ZMPT101B: Real-time Sine Wave ===");

  // Подсветка
  ledcAttach(TFT_BLK, 5000, 8);
  ledcWrite(TFT_BLK, 255);

  // SPI
  hspi->begin(TFT_SCLK, -1, TFT_MOSI, -1);

  // Дисплей
  tft.init(240, 240, SPI_MODE3);
  tft.setSPISpeed(40000000);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  // Canvas
  canvas = new GFXcanvas16(GRAPH_WIDTH, GRAPH_HEIGHT);
  if (canvas == nullptr) {
    Serial.println("ОШИБКА: не хватило памяти под canvas!");
    while (1) delay(1000);
  }

  // Калибровка датчика (поиск нулевой точки)
  calibrateSensor();

  // Настройка АЦП
  analogReadResolution(12); // 12-бит: 0..4095
  analogSetAttenuation(ADC_11db); // Диапазон ~0..3.3V

  drawHeader();
  Serial.println("Готово. Начинаем вывод...");
}

void loop() {
  // Собираем буфер сэмплов с фиксированным интервалом
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    unsigned long now = micros();
    // Ждем, пока не пройдет SAMPLE_INTERVAL_US
    while (micros() - now < SAMPLE_INTERVAL_US);
    
    rawData[i] = analogRead(ZMPT_PIN) - zeroPoint;
  }

  // Масштабируем сырые данные в диапазон высоты графика
  // Находим максимальную амплитуду в текущем буфере
  int16_t maxVal = 1; // Защита от деления на 0
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    if (abs(rawData[i]) > maxVal) maxVal = abs(rawData[i]);
  }
  
  // Масштабируем: центр графика = середина высоты
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    // Нормализуем к диапазону [-1, 1], затем к [0, GRAPH_HEIGHT-1]
    float normalized = (float)rawData[i] / maxVal;
    int16_t y = (GRAPH_HEIGHT / 2) + (normalized * (GRAPH_HEIGHT / 2 - 5));
    // Ограничиваем
    if (y < 0) y = 0;
    if (y > GRAPH_HEIGHT - 1) y = GRAPH_HEIGHT - 1;
    graphData[i] = y;
  }

  // Отрисовка в canvas
  drawGraphToCanvas();

  // Отправка на дисплей
  tft.drawRGBBitmap(0, GRAPH_TOP, canvas->getBuffer(), GRAPH_WIDTH, GRAPH_HEIGHT);

  // Периодический вывод в Serial
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    lastPrint = millis();
    // Вычисляем RMS для информации
    float sumSq = 0;
    for (int i = 0; i < SAMPLE_COUNT; i++) sumSq += (float)rawData[i] * rawData[i];
    float rms = sqrt(sumSq / SAMPLE_COUNT);
    Serial.print("Амплитуда (сырая): ");
    Serial.print(maxVal);
    Serial.print(", RMS (сырой): ");
    Serial.println(rms);
  }
}

// ===== Калибровка: поиск нулевой точки =====
void calibrateSensor() {
  Serial.println("Калибровка нулевой точки...");
  long sum = 0;
  int samples = 200;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(ZMPT_PIN);
    delayMicroseconds(100);
  }
  zeroPoint = sum / samples;
  Serial.print("Нулевая точка: ");
  Serial.println(zeroPoint);
}

// ===== Заголовок =====
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

// ===== Отрисовка графика в canvas =====
void drawGraphToCanvas() {
  canvas->fillScreen(ST77XX_BLACK);
  
  // Оси
  canvas->drawFastVLine(0, 0, GRAPH_HEIGHT, ST77XX_WHITE);
  canvas->drawFastHLine(0, GRAPH_HEIGHT - 1, GRAPH_WIDTH, ST77XX_WHITE);
  
  // Центральная линия (ноль)
  canvas->drawFastHLine(0, GRAPH_HEIGHT / 2, GRAPH_WIDTH, 0x4208);
  
  // Сетка (вертикальные линии каждые 60 пикселей = ~1/4 ширины)
  for (int x = 60; x < GRAPH_WIDTH; x += 60) {
    for (int y = 0; y < GRAPH_HEIGHT; y += 6) {
      canvas->drawPixel(x, y, 0x4208);
    }
  }

  // Кривая
  for (int x = 0; x < SAMPLE_COUNT - 1; x++) {
    int y1 = graphData[x];
    int y2 = graphData[x + 1];
    // Масштабируем X: SAMPLE_COUNT точек на всю ширину
    int x1 = (x * GRAPH_WIDTH) / SAMPLE_COUNT;
    int x2 = ((x + 1) * GRAPH_WIDTH) / SAMPLE_COUNT;
    canvas->drawLine(x1, y1, x2, y2, ST77XX_GREEN);
  }
}