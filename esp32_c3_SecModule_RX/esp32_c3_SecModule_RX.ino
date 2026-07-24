#include <esp_now.h>
#include <WiFi.h>

// Структура приёма (должна совпадать с отправителем)
typedef struct struct_message {
  uint8_t id;
  bool alarm;
} struct_message;
struct_message receivedData;

#define RELAY_PIN 8   // пин реле (можно заменить на встроенный светодиод GPIO8 для проверки)

// 🔥 НОВЫЙ ПРОТОТИП callback-функции приёма
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  // info->src_addr – MAC-адрес отправителя (может пригодиться для проверки)
  // info->des_addr – MAC-адрес получателя (обычно не используется)
  
  if (len == sizeof(receivedData)) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    
    Serial.print("Датчик #");
    Serial.print(receivedData.id);
    Serial.print(" (MAC: ");
    // Выведем MAC 
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X",info->src_addr[0], info->src_addr[1], info->src_addr[2], info->src_addr[3], info->src_addr[4], info->src_addr[5]);
    Serial.print("): ");
    Serial.println(receivedData.alarm ? "ТРЕВОГА!" : "Норма");

    // Управляем реле (или светодиодом)
    digitalWrite(RELAY_PIN, receivedData.alarm ? LOW : HIGH);
  } else {
    Serial.println("Получены данные неверного размера");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Ошибка инициализации ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Модуль управления запущен, ждём данные...");
}

void loop() {
  // Здесь можно разместить веб-сервер или другой код
}