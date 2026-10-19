#include <Arduino.h>

const unsigned long PERIOD_MS = 250;

// Функция подсчета контрольной суммы (XOR)
uint8_t calculateChecksum(const char *str) {
  uint8_t cs = 0;
  while (*str) {
    cs ^= (uint8_t)*str++;
  }
  return cs;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long last = 0;
  unsigned long now = millis();
  if (now - last < PERIOD_MS) return;
  last = now;

  // Заглушка температуры
  float t = 25.0f + 5.0f * sin(now / 3000.0f);
  int a = analogRead(A0);

  // Формируем полезную нагрузку
  char payload[32];
  snprintf(payload, sizeof(payload), "T=%.1f;A=%d", t, a);

  // Считаем контрольную сумму
  uint8_t cs = calculateChecksum(payload);

  // Отправляем: <payload>*<checksum_hex>\n
  Serial.print(payload);
  Serial.print('*');
  if (cs < 16) Serial.print('0'); // ведущий ноль для красоты
  Serial.print(cs, HEX);
  Serial.print('\n');
}