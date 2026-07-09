#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === Настройки дисплея ===
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === Пины ===
#define ANALOG_PIN    0   // GPIO0 = ADC0
#define ADC_MAX       4095.0
#define VREF          3.3

// === Параметры уровня ===
const float DB_MIN = -60.0;      // нижний предел шкалы (дБ)
const float DB_MAX = 0.0;        // верхний предел (0 дБ = полная шкала)
const float PEAK_DECAY_RATE = 10.0; // скорость спада пика (дБ/с)
const unsigned long PEAK_HOLD_MS = 1000; // время удержания пика (мс)

// === Глобальные переменные ===
float currentDB = DB_MIN;
float peakDB    = DB_MIN;
unsigned long peakHoldStart = 0;
bool peakHolding = false;

// Для сглаживания (RMS или пик огибающей)
const int SAMPLES = 10;          // число выборок для поиска пика
int sampleBuffer[SAMPLES];
int sampleIndex = 0;

// === Фильтрация и шумоподавление ===
const float SMOOTHING_FACTOR = 0.15;   // коэффициент сглаживания (0..1) – чем меньше, тем сильнее фильтр
const float NOISE_FLOOR_DB = -55.0;    // порог шума – ниже этого значения считаем "тишину"
float filteredDB = DB_MIN;             // сглаженное значение уровня

// === Преобразование напряжения в дБ (относительно полной шкалы) ===
float voltageToDB(float voltage) {
  if (voltage <= 0.0) return DB_MIN;
  float db = 20.0 * log10(voltage / VREF);
  if (db < DB_MIN) db = DB_MIN;
  if (db > DB_MAX) db = DB_MAX;
  return db;
}

// === Обновление пикового значения ===
void updatePeak(float newDB) {
  // Если новый уровень выше пика – обновляем пик и запускаем удержание
  if (newDB > peakDB) {
    peakDB = newDB;
    peakHoldStart = millis();
    peakHolding = true;
  }
  // Если пик удерживается дольше заданного времени – начинаем спад
  else if (peakHolding && (millis() - peakHoldStart > PEAK_HOLD_MS)) {
    // Спад со скоростью PEAK_DECAY_RATE дБ/с
    float decay = PEAK_DECAY_RATE * (millis() - peakHoldStart - PEAK_HOLD_MS) / 1000.0;
    peakDB = peakDB - decay;
    if (peakDB < newDB) peakDB = newDB; // не опускаем ниже текущего уровня
    if (peakDB < DB_MIN) {
      peakDB = DB_MIN;
      peakHolding = false;
    }
  }
}

// === Отрисовка интерфейса ===
void drawMeter(float levelDB, float peakDB) {
  display.clearDisplay();

  // 1. Рамка шкалы
  int barX = 5;
  int barY = 20;
  int barWidth = SCREEN_WIDTH - 10;   // 118 пикселей
  int barHeight = 20;

  display.drawRect(barX - 1, barY - 1, barWidth + 2, barHeight + 2, SSD1306_WHITE);

  // 2. Вычисляем позицию столбика (от DB_MIN до DB_MAX)
  float percent = constrain((levelDB - DB_MIN) / (DB_MAX - DB_MIN), 0.0, 1.0);
  int fillWidth = (int)(percent * barWidth);

  // 3. Рисуем столбик (градиент по цвету – от зелёного к красному)
  //    Для монохромного дисплея используем разную штриховку или просто белый
  if (fillWidth > 0) {
    display.fillRect(barX, barY, fillWidth, barHeight, SSD1306_WHITE);
  }

  // 4. Маркер пика (вертикальная линия)
  float peakPercent = constrain((peakDB - DB_MIN) / (DB_MAX - DB_MIN), 0.0, 1.0);
  int peakX = barX + (int)(peakPercent * barWidth);
  // Рисуем вертикальную линию на 2 пикселя выше и ниже столбика
  display.drawLine(peakX, barY - 2, peakX, barY + barHeight + 2, SSD1306_WHITE);
  // Можно добавить маленький треугольник или точку
  display.fillRect(peakX - 1, barY - 4, 3, 3, SSD1306_WHITE);

  // 5. Подписи шкалы (деления)
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(barX, barY + barHeight + 4);
  display.print("-60");
  display.setCursor(barX + barWidth / 4 - 6, barY + barHeight + 4);
  display.print("-40");
  display.setCursor(barX + barWidth / 2 - 6, barY + barHeight + 4);
  display.print("-20");
  display.setCursor(barX + barWidth - 12, barY + barHeight + 4);
  display.print("0");

  // 6. Цифровые значения (в дБ)
  display.setCursor(0, 0);
  display.print("Lvl: ");
  display.print(levelDB, 1);
  display.print(" dB");

  display.setCursor(0, 10);
  display.print("Peak: ");
  display.print(peakDB, 1);
  display.print(" dB");

  // 7. Если пик сброшен до минимума – покажем индикатор
  if (peakDB <= DB_MIN + 0.5) {
    display.setCursor(80, 10);
    display.print("(no signal)");
  }

  display.display();
}

// === setup ===
void setup() {
  Serial.begin(115200);

  // Инициализация I2C и дисплея
  Wire.begin(8, 9);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 not found!");
    while(1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  // Настройка АЦП
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); // 0..3.6В

  // Очистка буфера
  for (int i = 0; i < SAMPLES; i++) sampleBuffer[i] = 0;
  sampleIndex = 0;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Audio Level Meter");
  display.println("Ready...");
  display.display();
  delay(1000);
}

void loop() {
  // 1. Чтение АЦП с усреднением (50 выборок – убираем случайные выбросы)
  const int SAMPLES_AVG = 50;
  long sum = 0;
  for (int i = 0; i < SAMPLES_AVG; i++) {
    sum += analogRead(ANALOG_PIN);
  }
  int raw = sum / SAMPLES_AVG;   // среднее

  // 2. Пересчёт в напряжение и дБ
  float voltage = (raw / ADC_MAX) * VREF;
  float newDB = voltageToDB(voltage);

  // 3. Экспоненциальное сглаживание (фильтр)
  filteredDB = filteredDB + SMOOTHING_FACTOR * (newDB - filteredDB);

  // 4. Применяем шумовой порог
  float displayDB;
  if (filteredDB < NOISE_FLOOR_DB) {
    displayDB = DB_MIN;           // показываем -60 дБ (тишина)
  } else {
    displayDB = filteredDB;
  }

  // 5. Обновляем пик (используем сглаженное значение, но без порога? 
  //    Лучше использовать filteredDB для пика, но с ограничением, чтобы шум не создавал ложных пиков)
  //    Обновляем пик только если filteredDB > NOISE_FLOOR_DB, иначе не обновляем.
  if (filteredDB > NOISE_FLOOR_DB) {
    updatePeak(filteredDB);
  } else {
    // Если уровень ниже порога, то постепенно гасим пик (чтобы не завис на предыдущем значении)
    if (peakDB > NOISE_FLOOR_DB) {
      // Ускоренный спад до порога
      peakDB -= 20.0 * (millis() - peakHoldStart) / 1000.0;
      if (peakDB < NOISE_FLOOR_DB) peakDB = DB_MIN;
    }
  }

  // 6. Отрисовка (передаём displayDB и peakDB)
  drawMeter(displayDB, peakDB);

  // Отладка
  Serial.print("Raw avg: ");
  Serial.print(raw);
  Serial.print("  dB filtered: ");
  Serial.print(filteredDB, 1);
  Serial.print("  display: ");
  Serial.print(displayDB, 1);
  Serial.print("  Peak: ");
  Serial.println(peakDB, 1);

  delay(10); // частота ~100 Гц
}

// === loop ===
// void loop() {
//   // 1. Заполняем буфер выборками (определяем пик за период)
//   int raw = analogRead(ANALOG_PIN);
//   sampleBuffer[sampleIndex] = raw;
//   sampleIndex = (sampleIndex + 1) % SAMPLES;

//   // Находим максимальное значение в буфере (пик огибающей)
//   int maxRaw = 0;
//   for (int i = 0; i < SAMPLES; i++) {
//     if (sampleBuffer[i] > maxRaw) maxRaw = sampleBuffer[i];
//   }

//   // 2. Пересчёт в напряжение и дБ
//   float voltage = (maxRaw / ADC_MAX) * VREF;
//   float newDB = voltageToDB(voltage);

//   // 3. Обновляем текущий уровень (со сглаживанием – можно использовать фильтр)
//   // Для простоты берём newDB как есть, но можно добавить скользящее среднее
//   currentDB = newDB;

//   // 4. Обновляем пик
//   updatePeak(currentDB);

//   // 5. Отображение
//   drawMeter(currentDB, peakDB);

//   // 6. Отладка в Serial
//   Serial.print("Raw: ");
//   Serial.print(maxRaw);
//   Serial.print("  dB: ");
//   Serial.print(currentDB, 1);
//   Serial.print("  Peak: ");
//   Serial.println(peakDB, 1);

//   // Частота обновления ~ 100 Гц (10 мс) для хорошей реакции на звук
//   delay(10);
// }