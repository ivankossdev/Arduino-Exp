#include "config.h"
#include "fs_utils.h"
#include "wifi_manager.h"
#include "temperature_manager.h"
#include "server_manager.h"

const char* ssid = "";
const char* password = "";
const int ledPin = 8;
const int DEFAULT_TEMPERATURE = 21;
const unsigned long NONBLOCK_DELAY_MS = 2000;
const unsigned long WIFI_TIMEOUT_MS = 30000;

WebServer server(80);
String ledState;

int temperature = DEFAULT_TEMPERATURE;
unsigned long previousTempTime = 0;

WifiManager wifiMgr(ssid, password);
TemperatureManager tempMgr(DEFAULT_TEMPERATURE, 30, DEFAULT_TEMPERATURE);
ServerManager serverMgr(server);

String processor(const String& var) {
  if (var == "STATE") {
    ledState = digitalRead(ledPin) ? "ON" : "OFF";
    return ledState;
  }
  if (var == "TEMPERATURE") return String(tempMgr.get());
  if (var == "HUMIDITY") return "60.0";
  return String();
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nЗапуск ESP32-C3...");

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  if (!LittleFS.begin(true)) {
    Serial.println("ОШИБКА: Не удалось смонтировать LittleFS");
  } else {
    Serial.println("LittleFS смонтирована успешно.");
  }

  wifiMgr.begin();
  serverMgr.setupRoutes(processor);
  serverMgr.start();
}

void loop() {
  wifiMgr.tick();
  serverMgr.tick();

  unsigned long now = millis();
  if (now - previousTempTime >= NONBLOCK_DELAY_MS) {
    tempMgr.update();
    temperature = tempMgr.get();
    previousTempTime = now;
  }
}
