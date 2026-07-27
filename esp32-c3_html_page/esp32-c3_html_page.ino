#include "config.h"
#include "fs_utils.h"

// Реализация переменных
const char* ssid = "";
const char* password = "";
const int ledPin = 8;
const int DEFAULT_TEMPERATURE = 21;
const unsigned long NONBLOCK_DELAY_MS = 2000;
const unsigned long WIFI_TIMEOUT_MS = 30000; // 30 секунд на подключение

WebServer server(80);
String ledState;

int temperature = DEFAULT_TEMPERATURE;
unsigned long previousPrint = 0;

// Состояние Wi‑Fi (чтобы не блокировать loop)
enum WifiState {
  WIFI_NOT_STARTED,
  WIFI_CONNECTING,
  WIFI_CONNECTED,
  WIFI_FAILED
};

WifiState wifiState = WIFI_NOT_STARTED;
unsigned long wifiStartTime = 0;
bool wifiLoggedOnce = false;

// функция обновления температуры
int nextTemperature(int current) {
  if (current >= 30) {
    return DEFAULT_TEMPERATURE;
  }
  return current + 1;
}

// Процессор плейсхолдеров для HTML
String processor(const String& var) {
  if (var == "STATE") {
    ledState = digitalRead(ledPin) ? "ON" : "OFF";
    return ledState;
  }
  if (var == "TEMPERATURE") return String(temperature);
  if (var == "HUMIDITY") return "60.0";
  return String();
}

void setup() {
  Serial.begin(115200);
  // Никакого delay(1000) — сразу в работу
  Serial.println("\n\nЗапуск ESP32-C3...");

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  if (!LittleFS.begin(true)) {
    Serial.println("ОШИБКА: Не удалось смонтировать LittleFS");
  } else {
    Serial.println("LittleFS смонтирована успешно.");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  wifiState = WIFI_CONNECTING;
  wifiStartTime = millis();
  wifiLoggedOnce = false;

  // Роуты сервера
  server.on("/", HTTP_GET, []() {
    String content = getFileContent("/index.html");
    if (content.isEmpty()) {
      server.send(404, "text/plain", "File not found");
      return;
    }
    content.replace("%STATE%", processor("STATE"));
    content.replace("%TEMPERATURE%", processor("TEMPERATURE"));
    content.replace("%HUMIDITY%", processor("HUMIDITY"));
    server.send(200, "text/html", content);
  });

  server.on("/style.css", HTTP_GET, []() {
    sendFile(server, "/style.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, []() {
    sendFile(server, "/script.js", "application/javascript");
  });

  server.on("/data", HTTP_GET, []() {
    // Формируем JSON без лишних кавычек вокруг чисел — это правильный JSON
    String json = "{";
    json += "\"temperature\":" + String(temperature) + ",";
    json += "\"humidity\":60.0,";
    json += "\"state\":\"" + String(digitalRead(ledPin) ? "OFF" : "ON") + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });

  server.on("/on", HTTP_GET, []() {
    digitalWrite(ledPin, LOW);
    server.send(200, "text/plain", "OK");
  });

  server.on("/off", HTTP_GET, []() {
    digitalWrite(ledPin, HIGH);
    server.send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("HTTP-сервер запущен (ожидание Wi‑Fi).");
}

void loop() {
  // 1. Неблокирующая обработка Wi‑Fi
  if (wifiState == WIFI_CONNECTING) {
    if (!wifiLoggedOnce) {
      Serial.print("Подключение к Wi-Fi");
      wifiLoggedOnce = true;
    }

    unsigned long now = millis();
    // Печатаем точку каждые 500 мс БЕЗ delay()
    static unsigned long lastDotTime = 0;
    if (now - lastDotTime >= 500) {
      Serial.print(".");
      lastDotTime = now;
    }

    if (WiFi.status() == WL_CONNECTED) {
      wifiState = WIFI_CONNECTED;
      Serial.println();
      Serial.println("Подключено к Wi‑Fi!");
      Serial.print("IP-адрес: ");
      Serial.println(WiFi.localIP());
    } else if (now - wifiStartTime >= WIFI_TIMEOUT_MS) {
      wifiState = WIFI_FAILED;
      Serial.println();
      Serial.println("Не удалось подключиться к Wi‑Fi за таймаут.");
    }
  }

  // 2. Обработка HTTP-клиентов (это уже неблокирующе)
  server.handleClient();

  // 3. Неблокирующее обновление температуры
  unsigned long currentTime = millis();
  if (currentTime - previousPrint >= NONBLOCK_DELAY_MS) {
    temperature = nextTemperature(temperature);
    previousPrint = currentTime;
  }
}
