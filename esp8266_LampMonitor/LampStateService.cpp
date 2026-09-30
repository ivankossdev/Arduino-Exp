#include "LampStateService.h"

LampStateService::LampStateService(MqttService& mqttService)
    : _mqttService(mqttService)
{
    _mqttService.setOnConnected([this]() {
        this->onMqttConnected();
    });
}

void LampStateService::onMqttConnected() {
    // === ИЗМЕНЕНО: один короткий лог вместо развёрнутой строки ===
    Serial.println("[MQTT] Подписка на state-топик");
    _hasData = false;
    _mqttService.subscribeState();
}

bool LampStateService::handleMqttMessage(const String& topic, const String& payload) {
    const String& stateTopic = _mqttService.getCredentials().getStateTopic();
    if (topic != stateTopic) return false;

    // Дедупликация: одинаковые подряд идущие payload'ы игнорируем
    if (_hasData && _payload == payload) {
        return true;
    }

    _payload = payload;
    _hasData = true;

    // === ИЗМЕНЕНО: единственный содержательный лог — смена состояния лампы ===
    Serial.printf("[LAMP] %s\n", payload.c_str());

    if (_onChange) _onChange(_payload);
    return true;
}