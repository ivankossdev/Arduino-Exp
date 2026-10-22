#include <Arduino.h>
#include "MIK32Temp.h"

MIK32Temp chipTemp;

unsigned long telemetryRate = 1000; // Частота опроса (мс)
unsigned int seq = 0;

char rxBuf[32];
int rxIdx = 0;
unsigned long lastRxTime = 0;

uint8_t calcCRC8(const char *str, int len) {
  uint8_t crc = 0;
  for (int i = 0; i < len; i++) {
    crc ^= (uint8_t)str[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x80) crc = (crc << 1) ^ 0x07;
      else crc <<= 1;
    }
  }
  return crc;
}

void sendPacket(const char *payload) {
  uint8_t cs = calcCRC8(payload, strlen(payload));
  Serial.print(payload);
  Serial.print('*');
  if (cs < 16) Serial.print('0');
  Serial.print(cs, HEX);
  Serial.print('\n');
}

// Медианный фильтр для АЦП
int readFilteredADC(uint8_t pin) {
  int samples[5];
  for (int i = 0; i < 5; i++) {
    samples[i] = analogRead(pin);
    delayMicroseconds(100);
  }
  for (int i = 0; i < 4; i++) {
    for (int j = i + 1; j < 5; j++) {
      if (samples[i] > samples[j]) {
        int temp = samples[i];
        samples[i] = samples[j];
        samples[j] = temp;
      }
    }
  }
  return samples[2]; 
}

unsigned long lastSend = 0;
bool isMeasuring = false;
unsigned long measureStart = 0;

void setup() {
  Serial.begin(9600);
  if (!chipTemp.begin()) Serial.println("ERR:TSENS_init\n");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // --- 1. Чтение команд с ESP32 ---
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
          } 
          else if (strcmp(clean, "stat") == 0) {
            char statPayload[32];
            snprintf(statPayload, sizeof(statPayload), "STAT:up=%lus", millis() / 1000UL);
            sendPacket(statPayload);
          }
          else if (strcmp(clean, "led=1") == 0) {
            digitalWrite(LED_BUILTIN, HIGH);
            sendPacket("ACK:led");
          }
          else if (strcmp(clean, "led=0") == 0) {
            digitalWrite(LED_BUILTIN, LOW);
            sendPacket("ACK:led");
          }
          else if (strncmp(clean, "rate=", 5) == 0) {
            telemetryRate = atoi(clean + 5);
            if (telemetryRate < 50) telemetryRate = 50; // Защита от слишком частого опроса
            sendPacket("ACK:rate");
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

  if (!isMeasuring && (now - lastSend >= telemetryRate)) {
    lastSend = now;
    chipTemp.startRead(); 
    isMeasuring = true;
    measureStart = now;
  }

  if (isMeasuring) {
    if (chipTemp.isReady() || (now - measureStart > 500)) {
      float t = chipTemp.getResult();
      if (isnan(t)) t = 0.0f; 
      int a = readFilteredADC(A0); 

      seq++;
      char payload[64];
      snprintf(payload, sizeof(payload), "S=%u;T=%.2f;A=%d", seq, t, a);
      sendPacket(payload);
      
      isMeasuring = false; 
    }
  }
}