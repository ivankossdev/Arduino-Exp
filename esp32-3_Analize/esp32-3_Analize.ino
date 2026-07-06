#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "arduinoFFT.h"
#include "driver/adc.h"
#include "esp_adc/adc_continuous.h"

#define MIC_PIN ADC1_CHANNEL_0   // GPIO0
#define SAMPLES 256
#define SAMPLE_FREQ 10000

float vReal[SAMPLES];
float vImag[SAMPLES];
ArduinoFFT<float> FFT(vReal, vImag, SAMPLES, SAMPLE_FREQ);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 8
#define OLED_SCL 9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

static adc_continuous_handle_t adc_handle = NULL;
static const int BUFFER_SIZE = 2048;
volatile bool adc_data_ready = false;

float prevBands[8] = {0};
float peakBands[8] = {0};
int peakHold[8] = {0};

static bool IRAM_ATTR adc_callback(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data) {
    adc_data_ready = true;
    return true;
}

void initADC() {
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = BUFFER_SIZE,
        .conv_frame_size = 256,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &adc_handle));

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = SAMPLE_FREQ,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
    };
    adc_digi_pattern_config_t adc_pattern[1] = {
        {
            .atten = ADC_ATTEN_DB_11,
            .channel = MIC_PIN,
            .unit = ADC_UNIT_1,
            .bit_width = ADC_BITWIDTH_12,
        },
    };
    dig_cfg.adc_pattern = adc_pattern;
    dig_cfg.pattern_num = 1;
    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &dig_cfg));

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = adc_callback,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(adc_handle, &cbs, NULL));
}

void startSampling() { adc_continuous_start(adc_handle); }
void stopSampling() { adc_continuous_stop(adc_handle); }

void setup() {
    Serial.begin(115200);
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

    initADC();
    startSampling();
}

void loop() {
    if (adc_data_ready) {
        adc_data_ready = false;

        // --- Чтение данных (драйвер ещё работает) ---
        uint8_t result[BUFFER_SIZE];
        uint32_t ret_num = 0;
        esp_err_t err = adc_continuous_read(adc_handle, result, BUFFER_SIZE, &ret_num, 0);
        if (err != ESP_OK || ret_num == 0) {
            Serial.printf("ADC read error: %d, ret_num=%u\n", err, ret_num);
            // Попытка восстановления
            stopSampling();
            delay(10);
            startSampling();
            return;
        }

        // --- Останавливаем драйвер на время обработки ---
        stopSampling();

        // --- Преобразуем сырые данные в массив float ---
        int bytes_per_sample = 4; // Для ESP32-C3
        int sample_count = ret_num / bytes_per_sample;
        if (sample_count > SAMPLES) sample_count = SAMPLES;

        for (int i = 0; i < sample_count; i++) {
            adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i * bytes_per_sample];
            vReal[i] = p->type2.data;   // Используем type2 для ESP32-C3
            vImag[i] = 0.0;
        }
        // Заполняем остаток нулями, если данных меньше SAMPLES
        for (int i = sample_count; i < SAMPLES; i++) {
            vReal[i] = 0.0;
            vImag[i] = 0.0;
        }

        // --- Удаление DC смещения ---
        float mean = 0;
        for (int i = 0; i < SAMPLES; i++) mean += vReal[i];
        mean /= SAMPLES;
        for (int i = 0; i < SAMPLES; i++) vReal[i] -= mean;

        // --- FFT ---
        FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT.compute(FFT_FORWARD);
        FFT.complexToMagnitude();

        // --- Отображение ---
        displayBands();

        // --- Запускаем драйвер для следующего цикла ---
        startSampling();
    }
    delay(1);
}

void displayBands() {
  // Обнуляем самые низкие бины (остатки DC и гула)
  for (int i = 1; i <= 3; i++) {
    vReal[i] = 0;
  }

  // Находим глобальный максимум амплитуды (кроме нулевого бина)
  float maxVal = 0;
  for (int i = 1; i < SAMPLES / 2; i++) {
    if (vReal[i] > maxVal) maxVal = vReal[i];
  }
  if (maxVal < 1) maxVal = 1;

  // Массив максимумов для 8 полос
  float bandMax[8] = {0};

  // Группировка бинов в 8 полос (логарифмическая шкала)
  for (int i = 1; i < SAMPLES / 2; i++) {
    int band;
    if (i <= 4) band = 0;
    else if (i <= 8) band = 1;
    else if (i <= 12) band = 2;
    else if (i <= 18) band = 3;
    else if (i <= 30) band = 4;
    else if (i <= 50) band = 5;
    else if (i <= 80) band = 6;
    else band = 7;
    if (vReal[i] > bandMax[band]) bandMax[band] = vReal[i];
  }

  // --- Сглаживание (экспоненциальное) ---
  const float smoothFactor = 0.3; // 0.1–0.5
  for (int b = 0; b < 8; b++) {
    prevBands[b] = prevBands[b] * (1 - smoothFactor) + bandMax[b] * smoothFactor;
    bandMax[b] = prevBands[b];
  }

  // --- Пик-холдер (запоминание максимума) ---
  for (int b = 0; b < 8; b++) {
    if (bandMax[b] > peakBands[b]) {
      peakBands[b] = bandMax[b];
      peakHold[b] = 5; // держим пик 5 кадров
    } else if (peakHold[b] > 0) {
      peakHold[b]--;
    } else {
      peakBands[b] *= 0.98; // скорость затухания
    }
  }

  // --- Отрисовка ---
  display.clearDisplay();

  const int barWidth = 16;
  const int maxHeight = 50;

  for (int b = 0; b < 8; b++) {
    float norm = bandMax[b] / maxVal;
    int h = (int)(norm * maxHeight);
    if (h < 0) h = 0;
    if (h > maxHeight) h = maxHeight;

    int x = b * barWidth;
    int y = SCREEN_HEIGHT - h;

    // Столбец
    display.fillRect(x, y, barWidth - 2, h, WHITE);
    display.drawRect(x, y, barWidth - 2, h, BLACK);

    // Пиковая линия (маленькая горизонтальная чёрточка на уровне пика)
    float peakNorm = peakBands[b] / maxVal;
    int peakH = (int)(peakNorm * maxHeight);
    if (peakH > maxHeight) peakH = maxHeight;
    if (peakH > 0) {
      int peakY = SCREEN_HEIGHT - peakH;
      display.drawFastHLine(x, peakY, barWidth - 2, WHITE);
    }
  }
  display.display();
}
