#include <Arduino.h>
#include "MIK32Temp.h"

MIK32Temp chipTemp;

const unsigned long PERIOD_MS = 1000;  // 1 раз в секунду (т.к. чтение датчика долгое)
unsigned int seq = 0;

char rxBuf[32];
int rxIdx = 0;
unsigned long lastRxTime = 0;

// Заменяем старую функцию calcCS на эту:
uint8_t calcCRC8(const char *str, int len) {
  uint8_t crc = 0;
  for (int i = 0; i < len; i++) {
    crc ^= (uint8_t)str[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

// Отправка пакета с CRC
void sendPacket(const char *payload) {
  uint8_t cs = calcCRC8(payload, strlen(payload));
  Serial.print(payload);
  Serial.print('*');
  if (cs < 16) Serial.print('0');
  Serial.print(cs, HEX);
  Serial.print('\n');
}

// Медианный фильтр для АЦП (5 замеров)
int readFilteredADC(uint8_t pin) {
  int samples[5];
  // Делаем 5 быстрых замеров
  for (int i = 0; i < 5; i++) {
    samples[i] = analogRead(pin);
    delayMicroseconds(100);  // Микро-пауза для стабильности АЦП
  }

  // Сортируем массив методом пузырька (для 5 элементов это мгновенно)
  for (int i = 0; i < 4; i++) {
    for (int j = i + 1; j < 5; j++) {
      if (samples[i] > samples[j]) {
        int temp = samples[i];
        samples[i] = samples[j];
        samples[j] = temp;
      }
    }
  }

  // Возвращаем медиану (центральный элемент)
  return samples[2];
}

// Переменные для неблокирующего измерения
unsigned long lastSend = 0;
bool isMeasuring = false;
unsigned long measureStart = 0;

void setup() {
  Serial.begin(9600);

  if (!chipTemp.begin()) {
    Serial.println("ERR:TSENS_init\n");
  }

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // --- 1. Чтение команд с ESP32 (работает всегда!) ---
  while (Serial.available()) {
    char c = Serial.read();
    lastRxTime = millis();
    if (c == '\n') {
      rxBuf[rxIdx] = '\0';

      char *star = strrchr(rxBuf, '*');
      if (star) {
        int payloadLen = star - rxBuf;
        long expected = strtol(star + 1, NULL, 16);

        if (calcCRC8(rxBuf, payloadLen) == (uint8_t)expected) {
          char clean[32];
          strncpy(clean, rxBuf, payloadLen);
          clean[payloadLen] = '\0';

          if (strcmp(clean, "show") == 0) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            sendPacket("ACK:show");
          } else if (strcmp(clean, "stat") == 0) {
            char statPayload[32];
            unsigned long up_s = millis() / 1000UL;
            snprintf(statPayload, sizeof(statPayload), "STAT:up=%lus", up_s);
            sendPacket(statPayload);
          }
        }
      }
      rxIdx = 0;
    } else if (c != '\r') {
      if (rxIdx < sizeof(rxBuf) - 1) rxBuf[rxIdx++] = c;
    }
  }

  if (rxIdx > 0 && millis() - lastRxTime > 100) rxIdx = 0;

  // --- 2. Неблокирующее чтение датчика и отправка ---
  unsigned long now = millis();

  // Если пора отправлять и мы не ждем датчик — запускаем замер
  if (!isMeasuring && (now - lastSend >= PERIOD_MS)) {
    lastSend = now;
    chipTemp.startRead();  // Запустили в фоне
    isMeasuring = true;
    measureStart = now;
  }

  // Если замер идет — проверяем, не закончил ли датчик
  if (isMeasuring) {
    // Датчик готов ИЛИ прошло 500 мс (защита от зависания)
    if (chipTemp.isReady() || (now - measureStart > 500)) {
      float t = chipTemp.getResult();
      if (isnan(t)) t = 0.0f;

      int a = readFilteredADC(A0);  // Читаем АЦП с фильтрацией

      seq++;
      char payload[64];
      snprintf(payload, sizeof(payload), "S=%u;T=%.2f;A=%d", seq, t, a);
      sendPacket(payload);

      isMeasuring = false;  // Сбрасываем флаг, ждем следующей секунды
    }
  }
}