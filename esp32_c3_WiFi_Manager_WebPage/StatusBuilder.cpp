#include "StatusBuilder.h"
#include <ArduinoJson.h>

StatusBuilder::StatusBuilder(StateManager& stateManager,
                             WiFiService& wifiService,
                             MqttService& mqttService,
                             LedService& ledService)
    : _stateManager(stateManager),
      _wifiService(wifiService),
      _mqttService(mqttService),
      _ledService(ledService) {}

String StatusBuilder::buildJson() {
    // === ШАГ 1: сборка JSON через ArduinoJson вместо конкатенации строк ===
    // Раньше AppState::getStatusJson склеивал JSON вручную. Это работало, но
    // ломалось на спецсимволах (кавычки в SSID и т.п.) и плохо расширялось.
    JsonDocument doc;

    bool connected = _wifiService.isConnected();
    doc["connected"] = connected;
    if (connected) {
        doc["ssid"] = _wifiService.getCurrentSSID();
        doc["ip"]   = _wifiService.getIP().toString();
        doc["rssi"] = WiFi.RSSI();
    } else {
        doc["ssid"] = "";
        doc["ip"]   = "";
        doc["rssi"] = 0;
    }

    doc["led"]   = _ledService.isOn();
    doc["mqtt"]  = _mqttService.isConnected();
    doc["state"] = _stateManager.getStateString();

    String out;
    serializeJson(doc, out);
    return out;
}

String StatusBuilder::buildText() const {
    // === ШАГ 1: перенесено из AppState::getStatusString ===
    if (_wifiService.isConnected()) {
        String ssid = _wifiService.getCurrentSSID();
        IPAddress ip = _wifiService.getIP();
        return "Подключено к \"" + ssid + "\", IP: " + ip.toString();
    }
    return "Не подключено к Wi-Fi.";
}