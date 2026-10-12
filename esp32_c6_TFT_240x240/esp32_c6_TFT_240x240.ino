/*
  TFT ST7789 240x240 (SPI) + ESP32 Super Mini C6
  Выводит на дисплей приветствие и счётчик циклов.
  Подсветка управляется через GPIO0 с использованием ШИМ (новый API).

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
// #define TFT_CS   14   // Раскомментируйте, если на модуле есть CS

// Пин подсветки
#define TFT_BLK    0

// Частота ШИМ и разрешение
#define BLK_FREQ    5000
#define BLK_RES     8    // 8 бит → значения 0..255

// Инициализация дисплея (без CS)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK);

int counter = 0;

// Прототипы функций
void backlightOn();
void backlightOff();
void backlightSet(uint8_t level);   // 0..255

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("================================");
  Serial.println("ESP32 Super Mini C6 запущена!");
  Serial.println("Инициализация TFT ST7789 240x240...");
  Serial.println("================================");

  // --- Настройка ШИМ для подсветки (новый API Core 3.x) ---
  ledcAttach(TFT_BLK, BLK_FREQ, BLK_RES);
  backlightSet(255);   // Сразу включаем подсветку на полную

  // Инициализация дисплея
  tft.init(240, 240, SPI_MODE3);
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
  // === Мигаем подсветкой: плавное затухание ===
  Serial.println("Подсветка: затухание...");
  for (int b = 255; b >= 0; b -= 15) {
    backlightSet(b);
    delay(30);
  }
  Serial.println("Подсветка: разгорание...");
  for (int b = 0; b <= 255; b += 15) {
    backlightSet(b);
    delay(30);
  }

  // === Обновляем дисплей ===
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

// ===== Функции управления подсветкой =====

void backlightOn() {
  backlightSet(255);
}

void backlightOff() {
  backlightSet(0);
}

void backlightSet(uint8_t level) {
  ledcWrite(TFT_BLK, level);   // Новый API: указываем ПИН, а не канал
}