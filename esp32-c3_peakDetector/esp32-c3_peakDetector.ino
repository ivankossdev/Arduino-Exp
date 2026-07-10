#include "constants.h"
#include "Display.h"
#include "SignalProcessor.h"

void setup() {
    Serial.begin(115200);

    // Инициализация дисплея
    if (!initDisplay()) {
        Serial.println("SSD1306 not found!");
        while (1);
    }

    // Инициализация АЦП
    initADC();

    // Приветственное сообщение
    drawMeter(DB_MIN, DB_MIN);
    delay(1000);
}

void loop() {
    // 1. Получить текущий уровень (уже отфильтрованный и с порогом)
    float currentDB = getCurrentLevel();

    // 2. Обновить пик
    updatePeak(currentDB);

    // 3. Получить пик для отображения
    float peakDB = getPeak();

    // 4. Отрисовать на дисплее
    drawMeter(currentDB, peakDB);

    // 5. Отладка (опционально)
    Serial.print("Curr: ");
    Serial.print(currentDB, 1);
    Serial.print("  Peak: ");
    Serial.println(peakDB, 1);

    delay(10); // 100 Гц
}