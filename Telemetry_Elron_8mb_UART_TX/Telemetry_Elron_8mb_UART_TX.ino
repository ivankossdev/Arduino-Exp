#include <Arduino.h>
#include "MIK32Temp.h"

MIK32Temp chipTemp;

const unsigned long PERIOD_MS = 1000; // 1 раз в секунду (т.к. чтение датчика долгое)
unsigned int seq = 0;

char rxBuf[32];
int rxIdx = 0;
unsigned long lastRxTime = 0;

// Подсчет XOR для CRC
uint8_t calcCS(const char *str, int len) {
  uint8_t cs = 0;
  for (int i = 0; i < len; i++) cs ^= (uint8_t)str[i];
  return cs;
}

// Отправка пакета с CRC
void sendPacket(const char *payload) {
  uint8_t cs = calcCS(payload, strlen(payload));
  Serial.print(payload);
  Serial.print('*');
  if (cs < 16) Serial.print('0');
  Serial.print(cs, HEX);
  Serial.print('\n');
}

void setup() {
  Serial.begin(9600); // ВАЖНО: 9600, как на ESP32!
  
  if (!chipTemp.begin()) {
    // Если датчик не запустился, шлем аварийное сообщение
    // (ESP32 его выведет как [ERR] bad frame, так как нет CRC, 
    // но мы хотя бы увидим, что плата жива)
    Serial.println("ERR:TSENS_init\n");
  }
  
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
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            sendPacket("ACK:show");
          } 
          else if (strcmp(clean, "stat") == 0) {
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
  
  // Сброс буфера при таймауте
  if (rxIdx > 0 && millis() - lastRxTime > 100) rxIdx = 0;

  // --- 2. Отправка телеметрии ---
  static unsigned long last = 0;
  unsigned long now = millis();
  if (now - last < PERIOD_MS) return;
  last = now;
  
  seq++; // Увеличиваем номер пакета

  // Читаем температуру (усредняем 2 раза для скорости ~200мс)
  float t = chipTemp.readAveraged(2);
  
  // Если датчик вернул NAN (ошибка), шлем 0.0 или предыдущее значение
  if (isnan(t)) t = 0.0f; 

  // ADC на MIK32 (заглушка, если нет A0, шлем 0)
  int a = 0; 

  char payload[64];
  // Используем %.2f, так как датчик MIK32 дает сотые доли
  snprintf(payload, sizeof(payload), "S=%u;T=%.2f;A=%d", seq, t, a);
  sendPacket(payload);
}