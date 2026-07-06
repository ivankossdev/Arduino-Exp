#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "arduinoFFT.h"
#include "esp_timer.h"

// --- НАСТРОЙКИ ---
const int micPin = A3;          // АЦП-вход (GPIO0)
const int nSamples = 256;
const double sampleFreq = 10000.0; // Гц

// Буферы для FFT (float — быстрее на ESP32-C3)
float vReal[nSamples];
float vImag[nSamples];

ArduinoFFT<float> FFT(vReal, vImag, nSamples, sampleFreq);

// --- Дисплей ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 8
#define OLED_SCL 9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Переменные для выборки ---
volatile bool dataReady = false;
volatile int sampleIndex = 0;
const unsigned long samplePeriod_us = 1000000.0 / sampleFreq; // 100 мкс

esp_timer_handle_t timerHandle = NULL; // глобальный дескриптор таймера

// --- Обработчик прерывания таймера (корректная сигнатура) ---
void IRAM_ATTR onTimer(void *arg) {
  if (sampleIndex < nSamples) {
    vReal[sampleIndex] = analogRead(micPin);
    vImag[sampleIndex] = 0.0;
    sampleIndex++;
  } else {
    dataReady = true;
  }
}

// --- Запуск цикла выборки ---
void startSampling() {
  sampleIndex = 0;
  dataReady = false;
  esp_timer_start_periodic(timerHandle, samplePeriod_us);
}

// --- SETUP ---
void setup() {
  Serial.begin(115200);

  // I2C для OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("FFT Ready");
  display.display();

  // Создаём таймер
  esp_timer_create_args_t timerArgs = {
    .callback = onTimer,       // теперь сигнатура совпадает
    .arg = NULL,
    .name = "samplingTimer"
  };
  esp_timer_create(&timerArgs, &timerHandle);

  // Запускаем первую выборку
  startSampling();
}

// --- LOOP ---
void loop() {
  if (dataReady) {
    esp_timer_stop(timerHandle);

    // --- Удаление DC смещения ---
    float mean = 0;
    for (int i = 0; i < nSamples; i++) {
      mean += vReal[i];
    }
    mean /= nSamples;
    for (int i = 0; i < nSamples; i++) {
      vReal[i] -= mean;
    }

    // --- FFT ---
    FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(FFT_FORWARD);
    FFT.complexToMagnitude();

    displayBands();
    startSampling();
  }
  delay(5);
}

// --- Отрисовка спектра ---
void displayBands() {
    // Обнуляем самые низкие частоты (убираем остатки DC и гула)
    for (int i = 1; i <= 3; i++) {
        vReal[i] = 0;
    }

    // Ищем максимум для нормализации
    float maxVal = 0;
    for (int i = 1; i < nSamples / 2; i++) {
        if (vReal[i] > maxVal) maxVal = vReal[i];
    }
    if (maxVal < 1) maxVal = 1;

    float bandMax[7] = {0};

    // Новые границы полос (первая полоса начинается с бина 4)
    for (int i = 1; i < nSamples / 2; i++) {
        int band;
        if (i <= 4) band = 0;
        else if (i <= 8) band = 1;
        else if (i <= 12) band = 2;
        else if (i <= 20) band = 3;
        else if (i <= 35) band = 4;
        else if (i <= 55) band = 5;
        else band = 6;

        if (vReal[i] > bandMax[band]) bandMax[band] = vReal[i];
    }

    display.clearDisplay();

    const int barWidth = 16;
    const int maxHeight = 50;

    for (int b = 0; b < 7; b++) {
        float norm = bandMax[b] / maxVal;
        int h = (int)(norm * maxHeight);
        if (h < 0) h = 0;
        if (h > maxHeight) h = maxHeight;

        int x = b * barWidth;
        int y = SCREEN_HEIGHT - h;

        display.fillRect(x, y, barWidth - 2, h, WHITE);
        display.drawRect(x, y, barWidth - 2, h, BLACK);
    }
    display.display();
}