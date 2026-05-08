#include <WiFi.h>
#include <PubSubClient.h>

// ========== НАСТРОЙКИ  ==========
const char* ssid = "TechOtdel";          // Имя вашей домашней WiFi сети
const char* wifi_password = "12345678";     // Пароль от WiFi

const char* mqtt_server = "192.168.0.239";   // IP или домен вашего VPS
const int mqtt_port = 1883;                  // Порт MQTT (без TLS)
const char* mqtt_user = "esp32_user";        // Логин от Mosquitto
const char* mqtt_password = "1234"; // Пароль от Mosquitto

const char* cmd_topic = "home/lamp/command";   // Топик для команд (ON/OFF)
const char* state_topic = "home/lamp/status";  // Топик для публикации статуса

#define LAMP_PIN 8   // Встроенный светодиод на ESP32 
// =================================================

WiFiClient espClient;
PubSubClient client(espClient);

bool lampState = false;  // текущее состояние лампы

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("=== ESP32 MQTT Lamp Controller ===");

  pinMode(LAMP_PIN, OUTPUT);
  digitalWrite(LAMP_PIN, HIGH);  // начинаем с выключенной лампы
  lampState = false;

  // Подключение к WiFi
  Serial.print("Подключение к Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, wifi_password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi подключен. IP адрес: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Ошибка подключения к Wi-Fi! Проверьте SSID и пароль.");
  }

  // Настройка MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Получено сообщение в топике: ");
  Serial.println(topic);
  Serial.print("Сообщение: ");
  
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Проверяем, что топик - командный
  if (strcmp(topic, cmd_topic) == 0) {
    if (message == "ON") {
      lampState = true;
      digitalWrite(LAMP_PIN, LOW);
      Serial.println("Лампа ВКЛЮЧЕНА");
      client.publish(state_topic, "ON", true);
      Serial.println("Опубликован статус: ON");
    } 
    else if (message == "OFF") {
      lampState = false;
      digitalWrite(LAMP_PIN, HIGH);
      Serial.println("Лампа ВЫКЛЮЧЕНА");
      client.publish(state_topic, "OFF", true);
      Serial.println("Опубликован статус: OFF");
    }
    else {
      Serial.print("Неизвестная команда: ");
      Serial.println(message);
    }
  }
}

void reconnect() {
  int attempts = 0;
  while (!client.connected()) {
    Serial.print("Попытка подключения к MQTT брокеру...");
    if (client.connect("ESP32_Client", mqtt_user, mqtt_password)) {
      Serial.println(" УСПЕШНО!");
      // Подписываемся на командный топик
      if (client.subscribe(cmd_topic)) {
        Serial.print("Подписались на топик: ");
        Serial.println(cmd_topic);
      } else {
        Serial.println("Ошибка подписки на топик!");
      }
      // Опубликуем текущее состояние лампы (к этому моменту lampState = false)
      client.publish(state_topic, lampState ? "ON" : "OFF", true);
      Serial.print("Опубликован начальный статус: ");
      Serial.println(lampState ? "ON" : "OFF");
    } else {
      Serial.print(" Провал, rc=");
      Serial.print(client.state());
      Serial.println(" попытка через 5 секунд");
      attempts++;
      if (attempts > 5) {
        Serial.println("Слишком много неудачных попыток переподключения. Проверьте настройки MQTT (IP, логин, пароль).");
      }
      delay(5000);
    }
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi разорван, переподключаемся...");
    WiFi.begin(ssid, wifi_password);
    delay(5000);
    return;
  }
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Необязательно: мигаем встроенным светодиодом, показывая, что скетч работает
  // (можно раскомментировать для визуальной отладки, но лучше не мешать работе лампы)
  // static unsigned long lastBlink = 0;
  // if (millis() - lastBlink > 5000) {
  //   Serial.print(".");
  //   lastBlink = millis();
  // }
}