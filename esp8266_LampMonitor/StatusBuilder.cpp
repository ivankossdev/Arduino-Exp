#include "StatusBuilder.h"
#include <ArduinoJson.h>

StatusBuilder::StatusBuilder(StateManager& stateManager,
                             WiFiService& wifiService,
                             MqttService& mqttService,
                             LampStateService& lampStateService)
    : _stateManager(stateManager),
      _wifiService(wifiService),
      _mqttService(mqttService),
      _lampStateService(lampStateService) {}

String StatusBuilder::buildJson() {
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

    // === ИЗМЕНЕНО: поле led → lamp, значение берётся из LampStateService ===
    // Пустая строка, если данных ещё не было (или MQTT отключён).
    doc["lamp"]  = _lampStateService.hasData() ? _lampStateService.getPayload() : "";
    doc["mqtt"]  = _mqttService.isConnected();
    doc["state"] = _stateManager.getStateString();

    String out;
    serializeJson(doc, out);
    return out;
}

String StatusBuilder::buildText() const {
    if (_wifiService.isConnected()) {
        String ssid = _wifiService.getCurrentSSID();
        IPAddress ip = _wifiService.getIP();
        return "Подключено к \"" + ssid + "\", IP: " + ip.toString();
    }
    return "Не подключено к Wi-Fi.";
}