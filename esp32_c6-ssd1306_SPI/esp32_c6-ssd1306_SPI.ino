/*
  TFT ST7789 240x240 (SPI) + ESP32 Super Mini C6
  Аппаратный SPI + управление подсветкой через ШИМ.

  Подключение:
  TFT VCC  -> 3.3V
  TFT GND  -> GND
  TFT SCL  -> GPIO7 (CLK)
  TFT SDA  -> GPIO6 (MOSI)
  TFT RES  -> GPIO4
  TFT DC   -> GPIO15
  TFT BLK  -> GPIO0
  TFT CS   -> не используется (подключите к GND, если есть)
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Пины TFT
#define TFT_MOSI   6
#define TFT_SCLK   7
#define TFT_DC     15
#define TFT_RST    4
#define TFT_CS     -1     // -1, если на модуле нет вывода CS

// Пин подсветки
#define TFT_BLK    0

// Частота ШИМ и разрешение
#define BLK_FREQ    5000
#define BLK_RES     8

// --- Создаём объект SPIClass для аппаратного SPI ---
SPIClass* hspi = new SPIClass(SPI);

// --- ПРАВИЛЬНЫЙ конструктор для аппаратного SPI ---
// Порядок аргументов: (SPIClass*, CS, DC, RST)
Adafruit_ST7789 tft = Adafruit_ST7789(hspi, TFT_CS, TFT_DC, TFT_RST);

int counter = 0;

void backlightSet(uint8_t level) {
  ledcWrite(TFT_BLK, level);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("================================");
  Serial.println("ESP32 Super Mini C6 запущена!");
  Serial.println("Инициализация TFT ST7789 240x240 (аппаратный SPI)...");
  Serial.println("================================");

  // --- Настройка ШИМ для подсветки ---
  ledcAttach(TFT_BLK, BLK_FREQ, BLK_RES);
  backlightSet(255);

  // --- Инициализация аппаратного SPI с нашими пинами ---
  // Параметры: SCK, MISO, MOSI, SS
  hspi->begin(TFT_SCLK, -1, TFT_MOSI, -1);

  // --- Инициализация дисплея ---
  tft.init(240, 240, SPI_MODE3);
  tft.setSPISpeed(40000000);

  Serial.println("Дисплей инициализирован.");

  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  // Приветственный экран
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 20);
  tft.println("ESP32-C6");
  tft.setCursor(10, 50);
  tft.println("ST7789");
  tft.setCursor(10, 80);
  tft.println("240x240");
  tft.setCursor(10, 120);
  tft.setTextColor(ST77XX_GREEN);
  tft.println("STATUS: OK");
  tft.setCursor(10, 150);
  tft.setTextColor(ST77XX_CYAN);
  tft.print("COUNT: ");
  tft.println(counter);

  Serial.println("Первый вывод на экран завершён.");
  delay(2000);
}

void loop() {
  for (int b = 255; b >= 0; b -= 15) {
    backlightSet(b);
    delay(30);
  }
  for (int b = 0; b <= 255; b += 15) {
    backlightSet(b);
    delay(30);
  }

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 20);
  tft.println("ESP32-C6");
  tft.setCursor(10, 50);
  tft.println("ST7789");
  tft.setCursor(10, 80);
  tft.println("240x240");

  tft.setCursor(10, 120);
  tft.setTextColor(ST77XX_GREEN);
  tft.println("STATUS: OK");

  tft.setCursor(10, 150);
  tft.setTextColor(ST77XX_CYAN);
  tft.print("COUNT: ");
  tft.println(counter);

  Serial.print("Цикл #");
  Serial.println(counter);
  counter++;
}