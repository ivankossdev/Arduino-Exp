#include <HTTPClient.h>
#include <base64.h>
#include "WiFiManager.h"
#include "LedManager.h"

// Настройки Basic Auth
const char* AUTH_USERNAME = "myuser";
const char* AUTH_PASSWORD = "mypassword";

WiFiManager wifiManager;

LedManager ledPin(LED_BUILTIN);

void setup() {
  Serial.begin(115200);

  delay(1000);

  Serial.println("ESP32 starting...");

  if (wifiManager.begin()) {
    Serial.println("WiFi connected successfully!");
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

void loop() {
  if (wifiManager.isConnected()) {
    ledPin.On();
    HTTPClient http;
    http.begin(wifiManager.getServerURL());

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
  ledPin.Off();
  delay(10000); // Повторяем каждые 10 секунд
}
