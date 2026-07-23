#include <esp_now.h>
#include <WiFi.h>

// Структура приёма (должна совпадать с отправителем)
typedef struct struct_message {
  uint8_t id;
  bool alarm;
} struct_message;
struct_message receivedData;

// Пин реле (на C3 можно GPIO5)
#define RELAY_PIN 5

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("Датчик #");
  Serial.print(receivedData.id);
  Serial.print(": ");
  Serial.println(receivedData.alarm ? "ТРЕВОГА!" : "Норма");

  // Управляем реле
  digitalWrite(RELAY_PIN, receivedData.alarm ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Ошибка инициализации");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Модуль управления запущен");
}

void loop() {
  // Здесь можно добавить веб-сервер для ручного управления
}