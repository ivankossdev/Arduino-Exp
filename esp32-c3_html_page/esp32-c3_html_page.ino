#include "config.h"
#include "fs_utils.h"

// Определения переменных (extern)
const char* ssid = "TechOtdel";
const char* password = "12345678";
const int ledPin = 8;
WebServer server(80);
String ledState;

// Процессор плейсхолдеров
String processor(const String& var) {
    if (var == "STATE") {
        ledState = digitalRead(ledPin) ? "ON" : "OFF";
        return ledState;
    }
    if (var == "TEMPERATURE") return "25.5";
    if (var == "HUMIDITY") return "60.0";
    return String();
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\nЗапуск ESP32-C3...");

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);

    if (!LittleFS.begin(true)) {
        Serial.println("ОШИБКА: Не удалось смонтировать LittleFS");
        return;
    }
    Serial.println("LittleFS смонтирована успешно.");

    Serial.println("Список файлов в корне:");
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
        Serial.print("  - ");
        Serial.println(file.name());
        file = root.openNextFile();
    }

    WiFi.begin(ssid, password);
    Serial.print("Подключение к Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nПодключено к Wi-Fi!");
    Serial.print("IP-адрес: ");
    Serial.println(WiFi.localIP());

    // --- Настройка маршрутов (все в одном месте) ---

    // Главная страница
    server.on("/", HTTP_GET, []() {
        String content = getFileContent("/index.html", processor);
        if (content.isEmpty()) {
            server.send(404, "text/plain", "File not found");
            return;
        }
        // Заменяем плейсхолдеры вручную (можно вынести в отдельную функцию)
        content.replace("%STATE%", ledState);
        content.replace("%TEMPERATURE%", "25.5");
        content.replace("%HUMIDITY%", "60.0");
        server.send(200, "text/html", content);
    });

    // CSS
    server.on("/style.css", HTTP_GET, []() {
        sendFile(server, "/style.css", "text/css");
    });

    // JavaScript
    server.on("/script.js", HTTP_GET, []() {
        sendFile(server, "/script.js", "application/javascript");
    });

    // AJAX data
    server.on("/data", HTTP_GET, []() {
        String json = "{";
        json += "\"temperature\":\"25.5\",";
        json += "\"humidity\":\"60.0\",";
        json += "\"state\":\"" + String(digitalRead(ledPin) ? "OFF" : "ON") + "\"";
        json += "}";
        server.send(200, "application/json", json);
    });

    // Команды LED
    server.on("/on", HTTP_GET, []() {
        digitalWrite(ledPin, LOW);
        server.send(200, "text/plain", "OK");
    });

    server.on("/off", HTTP_GET, []() {
        digitalWrite(ledPin, HIGH);
        server.send(200, "text/plain", "OK");
    });

    server.begin();
    Serial.println("HTTP-сервер запущен.");
}

void loop() {
    server.handleClient();
}