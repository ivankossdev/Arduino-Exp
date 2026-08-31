#include "AppState.h"

AppState::AppState()
    : _stateManager(),
      _wifiService(_stateManager),
      _mqttService(_stateManager),
      _displayService(_stateManager, _wifiService, _mqttService, *this)   // Передаём ссылку на себя
{
    // Светодиод по умолчанию выключен
    _led.setMode(LED_OFF);

    // Регистрируем колбэк для MQTT-сообщений
    _mqttService.setMessageCallback([this](const String& topic, const String& payload) {
        this->handleMqttMessage(topic, payload);
    });
}

void AppState::begin() {
    _wifiService.begin();  // автоподключение Wi-Fi (без индикации)
    _displayService.begin();
}

String AppState::getStatusString() const {
    if (isConnected()) {
        String ssid = WiFi.SSID();
        IPAddress ip = WiFi.localIP();
        return "Подключено к \"" + ssid + "\", IP: " + ip.toString();
    } else {
        return "Не подключено к Wi-Fi.";
    }
}

// --- Светодиод ---
bool AppState::beginLed(int pin, bool activeLow) {
    bool ok = _led.begin(pin, activeLow);
    if (ok) {
        _led.setMode(LED_OFF);  // стартуем выключенным
    }
    return ok;
}

void AppState::updateLed() {
    _led.update();  // нужно для мигания (если режим изменится)
}

// --- MQTT обработка ---
void AppState::handleMqttMessage(const String& topic, const String& payload) {
    Serial.printf("📨 AppState: получено MQTT сообщение: топик=%s, payload=%s\n",
                  topic.c_str(), payload.c_str());

    // Управление светодиодом через MQTT
    if (topic == "home/lamp/command") {
        if (payload == "ON") {
            _led.setMode(LED_ON);
            _mqttService.publishState("ON");
        } else if (payload == "OFF") {
            _led.setMode(LED_OFF);
            _mqttService.publishState("OFF");
        }
        // Можно добавить другие команды (например, TOGGLE) при необходимости
    }
}

// --- Общий update ---
void AppState::update() {
    updateLed();        // обновление состояния светодиода (если мигает)
    updateMqtt();       // обновление MQTT-клиента
    _displayService.update();
}


void AppState::setLed(bool on) {
    _led.setMode(on ? LED_ON : LED_OFF);
}

bool AppState::getLedState() const {
    return _led.getState();
}

String AppState::getStatusJson() {
    String json = "{";
    json += "\"connected\":" + String(isConnected() ? "true" : "false");
    if (isConnected()) {
        json += ",\"ssid\":\"" + WiFi.SSID() + "\"";
        json += ",\"ip\":\"" + WiFi.localIP().toString() + "\"";
        json += ",\"rssi\":" + String(WiFi.RSSI());
    } else {
        json += ",\"ssid\":\"\"";
        json += ",\"ip\":\"\"";
        json += ",\"rssi\":0";
    }
    json += ",\"led\":" + String(getLedState() ? "true" : "false");
    json += ",\"mqtt\":" + String(_mqttService.isConnected() ? "true" : "false");
    json += ",\"state\":\"" + String(_stateManager.getStateString()) + "\"";
    json += "}";
    return json;
}