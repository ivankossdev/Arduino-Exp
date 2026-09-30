#include "AppState.h"

AppState::AppState()
    : _stateManager(),
      _wifiService(_stateManager),
      _mqttService(_stateManager),
      _lampStateService(_mqttService),   // === ИЗМЕНЕНО: вместо _ledService ===
      _displayService(_stateManager, _wifiService, _mqttService, _lampStateService),
      _statusBuilder(_stateManager, _wifiService, _mqttService, _lampStateService)
{
    // Регистрируем роутер входящих MQTT-сообщений
    _mqttService.setMessageCallback([this](const String& topic, const String& payload) {
        this->handleMqttMessage(topic, payload);
    });
}

void AppState::begin() {
    _wifiService.begin();
    _displayService.begin();
}

String AppState::getStatusString() const {
    return _statusBuilder.buildText();
}

// === УДАЛЕНО: beginLed / updateLed / setLed / getLedState ===

void AppState::handleMqttMessage(const String& topic, const String& payload) {
    Serial.printf("📨 AppState: MQTT: topic=%s, payload=%s\n",
                  topic.c_str(), payload.c_str());

    // === ИЗМЕНЕНО: делегируем в LampStateService вместо LedService ===
    if (_lampStateService.handleMqttMessage(topic, payload)) return;

    // Здесь в будущем можно добавить другие обработчики
}

void AppState::update() {
    // === ИЗМЕНЕНО: убран вызов _ledService.update() ===
    _mqttService.update();
    _displayService.update();
}

String AppState::getStatusJson() {
    return _statusBuilder.buildJson();
}