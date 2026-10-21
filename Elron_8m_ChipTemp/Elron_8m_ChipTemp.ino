#include "MIK32Temp.h"

MIK32Temp chipTemp;

void setup() {
    Serial.begin(115200);
    while (!Serial);

    if (!chipTemp.begin()) {
        Serial.println("TSENS init failed");
        while (1);
    }

    // При желании — разовая калибровка:
    // chipTemp.setOffset(-1.2f);  // если знаете, что датчик завышает на 1.2 °C

    Serial.println("MIK32Temp ready");
}

void loop() {
    // Одиночное чтение
    float t = chipTemp.read();
    if (isnan(t)) {
        Serial.println("read error");
    } else {
        Serial.print("Temp: ");
        Serial.print(t, 2);
        Serial.println(" °C");
    }

    // Или усреднённое (4 измерения × ~100 мс ≈ 400 мс)
    float avg = chipTemp.readAveraged(4);
    if (!isnan(avg)) {
        Serial.print("Avg : ");
        Serial.print(avg, 2);
        Serial.println(" °C");
    }

    delay(2000);
}