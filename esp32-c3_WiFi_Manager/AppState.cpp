#include "AppState.h"

AppState::AppState()
    : _stateManager(),
      _wifiService(_stateManager),
      _mqttService(_stateManager)
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
}