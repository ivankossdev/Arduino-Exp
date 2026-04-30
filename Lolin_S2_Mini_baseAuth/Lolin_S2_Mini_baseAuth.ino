// main.ino
#include <HTTPClient.h>
#include <base64.h>
#include "WiFiManager.h"

// Настройки Basic Auth
const char* AUTH_USERNAME = "myuser";
const char* AUTH_PASSWORD = "mypassword";
const char* SERVER_URL = "http://192.168.0.175:8090/auth";

WiFiManager wifiManager;

void setup() {
  Serial.begin(115200);

  // Запускаем менеджер WiFi — он либо подключится с сохранёнными данными,
  // либо запустит интерактивный режим настройки
  if (wifiManager.begin()) {
    Serial.println("WiFi connected successfully!");
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

void loop() {
  if (wifiManager.isConnected()) {
    HTTPClient http;
    http.begin(SERVER_URL);

    // Кодируем логин:пароль в Base64
    String authString = String(AUTH_USERNAME) + ":" + String(AUTH_PASSWORD);
    String base64Auth = base64::encode(authString);

    // Добавляем заголовок Authorization
    http.addHeader("Authorization", "Basic " + base64Auth);

    // Отправляем GET‑запрос
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("HTTP Code: " + String(httpCode));
      Serial.println("Response: " + payload);
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  } else {
    Serial.println("WiFi not connected, attempting reconnect...");
    wifiManager.begin(); // Пытаемся переподключиться
  }

  delay(10000); // Повторяем каждые 10 секунд
}
