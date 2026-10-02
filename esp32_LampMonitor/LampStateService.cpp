// ============================================================
// LampStateService — приём и хранение состояния лампы
// ============================================================
// ИЗМЕНЕНО: лог подписки → LOG_CAT_MQTT, лог изменения состояния →
//           LOG_CAT_LAMP. Теперь категории можно фильтровать отдельно.
// ============================================================

#include "LampStateService.h"
#include "log.h"       // ИЗМЕНЕНО: макросы логирования

LampStateService::LampStateService(MqttService& mqttService)
    : _mqttService(mqttService)
{
    _mqttService.setOnConnected([this]() {
        this->onMqttConnected();
    });
}

void LampStateService::onMqttConnected() {
    LOG_LN(LOG_CAT_MQTT, "[MQTT] Подписка на state-топик");
    _hasData = false;
    _mqttService.subscribeState();
}

bool LampStateService::handleMqttMessage(const String& topic, const String& payload) {
    const String& stateTopic = _mqttService.getCredentials().getStateTopic();
    if (topic != stateTopic) return false;

    if (_hasData && _payload == payload) {
        return true;
    }

    _payload = payload;
    _hasData = true;

    LOG(LOG_CAT_LAMP, "[LAMP] %s\n", payload.c_str());

    if (_onChange) _onChange(_payload);
    return true;
}