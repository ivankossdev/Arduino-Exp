#include "SignalProcessor.h"
#include "constants.h"
#include <Arduino.h>

// Статические переменные состояния
static float filteredDB = DB_MIN;      // сглаженное значение
static float peakDB = DB_MIN;          // текущий пик
static unsigned long peakHoldStart = 0;
static bool peakHolding = false;

void initADC() {
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);   // диапазон 0..3.6В
}

float getCurrentLevel() {
    // 1. Усреднение нескольких выборок
    long sum = 0;
    for (int i = 0; i < SAMPLES_AVG; i++) {
        sum += analogRead(ANALOG_PIN);
    }
    int raw = sum / SAMPLES_AVG;

    // 2. Преобразование в напряжение и дБ
    float voltage = (raw / ADC_MAX) * VREF;
    float newDB;
    if (voltage <= 0.0) {
        newDB = DB_MIN;
    } else {
        newDB = 20.0 * log10(voltage / VREF);
        if (newDB < DB_MIN) newDB = DB_MIN;
        if (newDB > DB_MAX) newDB = DB_MAX;
    }

    // 3. Экспоненциальный фильтр
    filteredDB = filteredDB + SMOOTHING_FACTOR * (newDB - filteredDB);

    // 4. Применяем шумовой порог
    if (filteredDB < NOISE_FLOOR_DB) {
        return DB_MIN;   // показываем минимум
    } else {
        return filteredDB;
    }
}

void updatePeak(float currentDB) {
    // Обновляем пик, если текущий уровень выше и он выше шумового порога
    if (currentDB > peakDB && currentDB > NOISE_FLOOR_DB) {
        peakDB = currentDB;
        peakHoldStart = millis();
        peakHolding = true;
    }

    // Если пик удерживается дольше PEAK_HOLD_MS – начинаем спад
    if (peakHolding && (millis() - peakHoldStart > PEAK_HOLD_MS)) {
        float decay = PEAK_DECAY_RATE * (millis() - peakHoldStart - PEAK_HOLD_MS) / 1000.0;
        peakDB = peakDB - decay;
        if (peakDB < currentDB) peakDB = currentDB; // не ниже текущего уровня
        if (peakDB < NOISE_FLOOR_DB) {
            peakDB = DB_MIN;
            peakHolding = false;
        }
    }

    // Если сигнал упал ниже порога, ускоряем спад пика
    if (currentDB <= NOISE_FLOOR_DB && peakDB > NOISE_FLOOR_DB) {
        peakDB -= 20.0 * (millis() - peakHoldStart) / 1000.0;
        if (peakDB < NOISE_FLOOR_DB) {
            peakDB = DB_MIN;
            peakHolding = false;
        }
    }
}

float getPeak() {
    return peakDB;
}

void resetPeak() {
    peakDB = DB_MIN;
    peakHolding = false;
    peakHoldStart = millis();
}