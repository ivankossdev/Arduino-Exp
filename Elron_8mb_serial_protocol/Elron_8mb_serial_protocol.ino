#include <Arduino.h>

const unsigned long PERIOD_MS = 250;
unsigned int seq = 0;

char rxBuf[32];
int rxIdx = 0;
unsigned long lastRxTime = 0;

uint8_t calcCS(const char *str, int len) {
  uint8_t cs = 0;
  for (int i = 0; i < len; i++) cs ^= (uint8_t)str[i];
  return cs;
}

void sendPacket(const char *payload) {
  uint8_t cs = calcCS(payload, strlen(payload));
  Serial.print(payload);
  Serial.print('*');
  if (cs < 16) Serial.print('0');
  Serial.print(cs, HEX);
  Serial.print('\n');
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // --- 1. Чтение команд с ESP32 ---
  while (Serial.available()) {
    char c = Serial.read();
    lastRxTime = millis();
    if (c == '\n') {
      rxBuf[rxIdx] = '\0';
      // Проверяем CRC входящей команды
      char *star = strrchr(rxBuf, '*');
      if (star) {
        int payloadLen = star - rxBuf;
        long expected = strtol(star + 1, NULL, 16);
        if (calcCS(rxBuf, payloadLen) == (uint8_t)expected) {
          char clean[32];
          strncpy(clean, rxBuf, payloadLen);
          clean[payloadLen] = '\0';
          
          if (strcmp(clean, "show") == 0) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Мигаем
            sendPacket("ACK:show"); // Отвечаем
          }
        }
      }
      rxIdx = 0;
    } else if (c != '\r') {
      if (rxIdx < sizeof(rxBuf) - 1) rxBuf[rxIdx++] = c;
    }
  }
  // Сброс буфера при таймауте
  if (rxIdx > 0 && millis() - lastRxTime > 100) rxIdx = 0;

  // --- 2. Отправка телеметрии ---
  static unsigned long last = 0;
  unsigned long now = millis();
  if (now - last < PERIOD_MS) return;
  last = now;
  
  seq++; // Увеличиваем номер пакета

  float t = 25.0f + 5.0f * sin(now / 3000.0f);
  int a = analogRead(A0);

  char payload[64];
  snprintf(payload, sizeof(payload), "S=%u;T=%.1f;A=%d", seq, t, a);
  sendPacket(payload);
}