/*
  OLED SSD1306 (SPI) + ESP32 Super Mini C6
  Выводит на дисплей приветствие и счётчик циклов.
  
  Подключение (Software SPI):
  OLED VCC  -> 3.3V
  OLED GND  -> GND
  OLED D0   -> GPIO7 (CLK)
  OLED D1   -> GPIO5 (MOSI)
  OLED RES  -> GPIO4
  OLED DC   -> GPIO2
  OLED CS   -> GPIO3
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Размеры дисплея
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Определение пинов для программного SPI
#define OLED_MOSI   5
#define OLED_CLK    7
#define OLED_DC     2
#define OLED_CS     3
#define OLED_RESET  4

// Инициализация дисплея (программный SPI)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Переменная для счётчика
int counter = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Инициализация OLED SSD1306 (SPI)...");

  // Инициализация дисплея
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("Ошибка: Не удалось найти SSD1306!");
    for (;;); // Остановка, если дисплей не найден
  }

  Serial.println("Дисплей найден. Начинаю работу...");

  // Очистка буфера
  display.clearDisplay();
  
  // Настройка текста
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Первый вывод
  display.println("ESP32-C6 Super Mini");
  display.println("OLED SSD1306 (SPI)");
  display.println("---------------------");
  display.println("Status: OK");
  display.println("Counter: 0");
  display.display();
  
  delay(2000);
}

void loop() {
  // Очищаем экран
  display.clearDisplay();
  
  // Устанавливаем курсор
  display.setCursor(0, 0);
  
  // Выводим статичный заголовок
  display.println("ESP32-C6 Super Mini");
  display.println("OLED SSD1306 (SPI)");
  display.println("---------------------");
  
  // Выводим статус и счётчик
  display.print("Status: ");
  display.println("OK");
  display.print("Counter: ");
  display.println(counter);

  // Отправляем буфер на дисплей
  display.display();

  // Вывод в консоль
  Serial.print("Цикл #");
  Serial.println(counter);

  counter++;
  delay(1000); // Обновление раз в секунду
}