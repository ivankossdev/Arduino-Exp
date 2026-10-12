/*
  TFT ST7789 240x240 + ESP32 Super Mini C6
  Плавный график в реальном времени с двойной буферизацией.

  Подключение:
  TFT VCC  -> 3.3V
  TFT GND  -> GND
  TFT SCL  -> GPIO7 (CLK)
  TFT SDA  -> GPIO6 (MOSI)
  TFT RES  -> GPIO4
  TFT DC   -> GPIO15
  TFT BLK  -> GPIO0
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Пины TFT
#define TFT_MOSI   6
#define TFT_SCLK   7
#define TFT_DC     15
#define TFT_RST    4
#define TFT_CS     -1

// Пин подсветки
#define TFT_BLK    0
#define BLK_FREQ   5000
#define BLK_RES    8

// --- Параметры графика ---
#define GRAPH_WIDTH   240
#define GRAPH_HEIGHT  180
#define GRAPH_TOP     40
#define GRAPH_BOTTOM  (GRAPH_TOP + GRAPH_HEIGHT)

#define SAMPLE_INTERVAL 20

// Объекты
SPIClass* hspi = new SPIClass(SPI);
Adafruit_ST7789 tft = Adafruit_ST7789(hspi, TFT_CS, TFT_DC, TFT_RST);

// --- Двойной буфер для области графика ---
GFXcanvas16* canvas = nullptr;

// Буфер значений
int16_t graphData[GRAPH_WIDTH];

unsigned long lastSample = 0;
uint32_t sampleCount = 0;

// Прототипы
void drawGraphHeader();
void drawGraphToCanvas();

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== ST7789: Smooth real-time graph ===");

  // Подсветка
  ledcAttach(TFT_BLK, BLK_FREQ, BLK_RES);
  ledcWrite(TFT_BLK, 255);

  // Аппаратный SPI
  hspi->begin(TFT_SCLK, -1, TFT_MOSI, -1);

  // Дисплей
  tft.init(240, 240, SPI_MODE3);
  tft.setSPISpeed(40000000);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  // --- Выделяем память под canvas ---
  canvas = new GFXcanvas16(GRAPH_WIDTH, GRAPH_HEIGHT);
  if (canvas == nullptr || canvas->getBuffer() == nullptr) {
    Serial.println("ОШИБКА: не хватило памяти под canvas!");
    while (1) delay(1000);
  }
  Serial.print("Canvas OK: ");
  Serial.print(GRAPH_WIDTH * GRAPH_HEIGHT * 2);
  Serial.println(" байт выделено.");

  // Инициализация буфера средними значениями
  for (int i = 0; i < GRAPH_WIDTH; i++) {
    graphData[i] = GRAPH_HEIGHT / 2;
  }

  // Заголовок рисуем прямо на дисплее (он статичный)
  drawGraphHeader();

  // Первый кадр
  drawGraphToCanvas();
  tft.drawRGBBitmap(0, GRAPH_TOP, canvas->getBuffer(),
                    GRAPH_WIDTH, GRAPH_HEIGHT);

  Serial.println("Готово. Начинаем вывод...");
}

void loop() {
  if (millis() - lastSample < SAMPLE_INTERVAL) return;
  lastSample = millis();
  sampleCount++;

  // Сдвигаем буфер влево
  memmove(&graphData[0], &graphData[1],
          (GRAPH_WIDTH - 1) * sizeof(int16_t));

  // Генерируем новое значение (random walk)
  int16_t last = graphData[GRAPH_WIDTH - 2];
  int16_t delta = random(-15, 16);
  int16_t newVal = last + delta;
  if (newVal < 0) newVal = 0;
  if (newVal > GRAPH_HEIGHT - 1) newVal = GRAPH_HEIGHT - 1;
  graphData[GRAPH_WIDTH - 1] = newVal;

  // Собираем кадр в памяти (без обращения к дисплею)
  drawGraphToCanvas();

  // Отправляем одним махом на дисплей — никакого мерцания
  tft.drawRGBBitmap(0, GRAPH_TOP, canvas->getBuffer(),
                    GRAPH_WIDTH, GRAPH_HEIGHT);

  // Периодически обновляем счётчик в заголовке
  if (sampleCount % 25 == 0) {
    drawGraphHeader();
    // Serial.print("Точек: ");
    // Serial.print(sampleCount);
    // Serial.print(", значение: ");
    // Serial.println(newVal);
  }
}

// ===== Заголовок (рисуется прямо на tft, редко меняется) =====
void drawGraphHeader() {
  tft.fillRect(0, 0, 240, GRAPH_TOP, ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 5);
  tft.print("ADC Real-time");

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(5, 25);
  tft.print("Samples: ");
  tft.print(sampleCount);
}

// ===== Рисуем график в canvas (в памяти) =====
void drawGraphToCanvas() {
  // Очищаем canvas
  canvas->fillScreen(ST77XX_BLACK);

  // Оси
  canvas->drawFastVLine(0, 0, GRAPH_HEIGHT, ST77XX_WHITE);
  canvas->drawFastHLine(0, GRAPH_HEIGHT - 1, GRAPH_WIDTH, ST77XX_WHITE);

  // Пунктирная сетка каждые 45 пикселей
  for (int y = 45; y < GRAPH_HEIGHT - 1; y += 45) {
    for (int x = 0; x < GRAPH_WIDTH; x += 6) {
      canvas->drawPixel(x, y, 0x4208);   // тёмно-серый
    }
  }

  // Кривая
  for (int x = 0; x < GRAPH_WIDTH - 1; x++) {
    int y1 = GRAPH_HEIGHT - 1 - graphData[x];
    int y2 = GRAPH_HEIGHT - 1 - graphData[x + 1];
    canvas->drawLine(x, y1, x + 1, y2, ST77XX_GREEN);
  }
}