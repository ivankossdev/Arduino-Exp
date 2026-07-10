#ifndef CONSTANTS_H
#define CONSTANTS_H

// === Настройки дисплея SSD1306 ===
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1      // Если не используется аппаратный сброс

// === Пины ===
#define ANALOG_PIN     0   // GPIO0 = ADC0
#define OLED_SDA       8    // I2C SDA
#define OLED_SCL       9    // I2C SCL
#define OLED_ADDR      0x3C // Адрес SSD1306 (чаще всего 0x3C)

// === Параметры АЦП ===
#define ADC_MAX        4095.0  // 12 бит
#define VREF           3.3     // Опорное напряжение (макс. входное)

// === Шкала уровня (дБ) ===
const float DB_MIN = -60.0;
const float DB_MAX = 0.0;

// === Параметры фильтрации и шумоподавления ===
const float SMOOTHING_FACTOR = 0.15;   // 0..1, меньше = сильнее сглаживание
const float NOISE_FLOOR_DB = -55.0;    // ниже этого уровня считаем тишину

// === Параметры пикового детектора ===
const float PEAK_DECAY_RATE = 10.0;    // дБ/с (скорость спада пика)
const unsigned long PEAK_HOLD_MS = 500; // мс удержания пика перед спадом

// === Параметры усреднения АЦП ===
const int SAMPLES_AVG = 50;            // число выборок для усреднения

#endif