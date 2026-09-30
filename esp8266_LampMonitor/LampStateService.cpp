#include "LampStateService.h"

LampStateService::LampStateService(MqttService& mqttService)
    : _mqttService(mqttService)
{
    _mqttService.setOnConnected([this]() {
        this->onMqttConnected();
    });
}

void LampStateService::onMqttConnected() {
    Serial.println("🔔 MQTT (пере)подключён — подписываемся на state-топик");

    // === ИСПРАВЛЕНИЕ: сбрасываем признак «данные есть» при (пере)подключении ===
    // Это гарантирует, что следующее принятое значение (даже если оно
    // совпадает с предыдущим) обновит дисплей. Между подпиской и приходом
    // retained экран покажет «LAMP: ?» — это честное отображение состояния
    // «данные ещё не пришли».
    _hasData = false;

    _mqttService.subscribeState();
}

bool LampStateService::handleMqttMessage(const String& topic, const String& payload) {
    const String& stateTopic = _mqttService.getCredentials().getStateTopic();
    if (topic != stateTopic) return false;

    // === ИСПРАВЛЕНИЕ: дедупликация одинаковых последовательных сообщений ===
    // Некоторые брокеры (в частности broker.hivemq.com) при определённых
    // условиях доставляют одно и то же сообщение дважды — например, если
    // подписка была зарегистрирована дважды в рамках одной сессии.
    // Игнорируем подряд идущие одинаковые payload'ы, чтобы не дёргать
    // дисплей и не нагружать Serial лишними строками.
    // Важно: проверка идёт только когда данные уже есть (_hasData == true).
    // После (пере)подключения флаг сбрасывается, и первое сообщение
    // применится, даже если его значение совпадает с прежним.
    if (_hasData && _payload == payload) {
        return true;   // сообщение наше, но это повтор — обработали без действий
    }

    _payload = payload;
    _hasData = true;
    Serial.printf("💡 Lamp state = \"%s\"\n", payload.c_str());

    if (_onChange) _onChange(_payload);
    return true;
}