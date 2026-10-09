#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Разрешение дисплея (часто 128x64)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Адрес I2C: обычно 0x3C, но бывает и 0x3D — проверь сканером I2C
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void setup() {
  Serial.begin(9600);
  while (!Serial); // ждём подключения монитора порта (если нужно)

  // Инициализация дисплея
  if (!display.begin(OLED_ADDR, true)) {
    Serial.println(F("Ошибка инициализации дисплея! Проверьте подключение и адрес."));
    for (;;); // зависаем, если дисплей не найден
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.println("ELBEAR ACE-UNO");
  display.println("SSD1306 test");
  display.println("I2C OK");

  // Рисуем рамку
  display.drawRect(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, SSD1306_WHITE);

  // Ставим точку в центре
  display.drawPixel(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SSD1306_WHITE);

  display.display();
}

void loop() {
  // Здесь можно добавить анимацию, вывод данных с датчиков и т.д.
  delay(2000);
}
