#include "LampStateService.h"

LampStateService::LampStateService(MqttService& mqttService)
    : _mqttService(mqttService)
{
    // === НОВОЕ: регистрируемся на (пере)подключение к брокеру ===
    // При коннекте подпишемся на state-топик и получим retained-значение.
    _mqttService.setOnConnected([this]() {
        this->onMqttConnected();
    });
}

void LampStateService::onMqttConnected() {
    // === НОВОЕ: вместо сложной retained-синхронизации из LedService ===
    // просто подписываемся на топик. Retained придёт сам, если он есть.
    Serial.println("🔔 MQTT (пере)подключён — подписываемся на state-топик");
    _mqttService.subscribeState();
}

bool LampStateService::handleMqttMessage(const String& topic, const String& payload) {
    const String& stateTopic = _mqttService.getCredentials().getStateTopic();
    if (topic != stateTopic) return false;

    _payload = payload;
    _hasData = true;
    Serial.printf("💡 Lamp state = \"%s\"\n", payload.c_str());

    if (_onChange) _onChange(_payload);
    return true;
}