#include <esp_now.h>
#include <WiFi.h>
#include <Bounce2.h>

// Передатчик событий
// 0x90, 0xE5, 0xB1, 0x6A, 0x8C, 0x1C адрес приемного контроллера

uint8_t broadcastAddress[] = { 0x90, 0xE5, 0xB1, 0x6A, 0x8C, 0x1C };

typedef struct struct_message {
  uint8_t id;
  bool alarm;
} struct_message;
struct_message myData;

Bounce pressButton = Bounce();

#define SENSOR_PIN 9  // GPIO9(кнопка BOOT)

// 🔥 НОВЫЙ ПРОТОТИП callback-функции (для новых версий ESP32)
void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  Serial.print("Статус отправки: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Успешно" : "Ошибка");
}

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Ошибка инициализации ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);  // регистрация та же

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Ошибка добавления пира");
    return;
  }
  Serial.println("Модуль охраны готов");

  // Обработчик нажатий кнопки
  pressButton.attach(SENSOR_PIN);
  pressButton.interval(25);
}

unsigned long previousPrint = 0;
const int NONBLOCKDELAY = 1000;
bool alarmState = false;

void loop() {
  pressButton.update();
  if (pressButton.fell()) {
    if (!alarmState) {
      alarmState = true;
      myData.id = 1;
      myData.alarm = alarmState;
    } else {
      alarmState = false;
      myData.id = 1;
      myData.alarm = alarmState;
    }
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  }
}