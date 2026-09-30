#include "AppState.h"

AppState::AppState()
    : _stateManager(),
      _wifiService(_stateManager),
      _mqttService(_stateManager),
      _lampStateService(_mqttService),
      _displayService(_stateManager, _wifiService, _mqttService, _lampStateService),
      _statusBuilder(_stateManager, _wifiService, _mqttService, _lampStateService)
{
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

void AppState::handleMqttMessage(const String& topic, const String& payload) {
  
    // === ИЗМЕНЕНО: убран отладочный лог на каждое сообщение ===
    if (_lampStateService.handleMqttMessage(topic, payload)) return;
}

void AppState::update() {
    _mqttService.update();
    _displayService.update();
}

String AppState::getStatusJson() {
    return _statusBuilder.buildJson();
}