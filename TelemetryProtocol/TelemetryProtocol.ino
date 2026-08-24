#include "TelemetryProtocol.h"

// Глобальный объект протокола (используем Serial)
TelemetryProtocol proto(Serial, true);  // со стаффингом

// Обработчик принятых пакетов (для всех ID можно сделать общий,
// но здесь покажем привязку к конкретным ID)
void onDigital(uint8_t id, uint8_t seq, const uint8_t* data, uint8_t len) {
    if (len >= 2) {
        uint16_t ports = data[0] | (data[1] << 8);
        Serial.print("Digital ports received: 0x");
        Serial.println(ports, HEX);
    }
}

void onStatus(uint8_t id, uint8_t seq, const uint8_t* data, uint8_t len) {
    if (len >= 5) {
        uint8_t flags = data[0];
        int16_t temp = data[1] | (data[2] << 8);
        uint16_t volt = data[3] | (data[4] << 8);
        Serial.print("Status: flags="); Serial.print(flags);
        Serial.print(" temp="); Serial.print(temp / 10.0);
        Serial.print("C volt="); Serial.print(volt); Serial.println("mV");
    }
}

void setup() {
    Serial.begin(115200);
    
    // Регистрируем обработчики для нужных ID
    proto.setHandler(PID_DIGITAL, onDigital);
    proto.setHandler(PID_STATUS, onStatus);
    // Для остальных ID можно не назначать – они будут игнорироваться
}

void loop() {
    // Отправка телеметрии (например, раз в 200 мс)
    static uint32_t lastSend = 0;
    if (millis() - lastSend > 200) {
        lastSend = millis();

        // Пример: отправляем состояние 16 цифровых портов
        uint16_t ports = 0b1010101010101010;
        proto.sendDigital(ports);

        // Отправляем статус: флаги 0x01, температура 23.5°C (235), напряжение 3300 мВ
        proto.sendStatus(0x01, 235, 3300);

        // Можно отправить аналоговые данные (например, с АЦП)
        // uint16_t adc[4] = {512, 1023, 0, 128};
        // proto.sendAnalog(adc, 4);
    }

    // Приём: читаем все доступные байты из Serial
    while (Serial.available()) {
        uint8_t b = Serial.read();
        proto.processByte(b);
    }
}