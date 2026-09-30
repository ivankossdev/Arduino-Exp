#include "MqttService.h"

MqttService::MqttService(StateManager& stateManager)
    : _stateManager(stateManager), _messageCallback(nullptr),
      _connectedCallback(nullptr), _callbacksRegistered(false) {}

bool MqttService::begin() {
    if (!loadCredentials()) {
        Serial.println("[MQTT] Настройки не найдены");
        return false;
    }

    String server = _credentials.getServer();
    int port = _credentials.getPort();
    String user = _credentials.getUser();
    String password = _credentials.getPassword();
    String stateTopic = _credentials.getStateTopic();

    if (server.length() == 0) {
        Serial.println("[MQTT] Сервер не задан");
        return false;
    }

    _manager.disconnect();

    bool result = _manager.begin(server, port, user, password, stateTopic);
    if (result) {
        if (!_callbacksRegistered) {
            MqttManager::setCallback([this](const String& topic, const String& payload) {
                this->handleMessage(topic, payload);
            });

            MqttManager::setOnConnected([this]() {
                if (this->_connectedCallback) {
                    this->_connectedCallback();
                }
            });

            _callbacksRegistered = true;
        }
        // === ИЗМЕНЕНО: убран лог "MQTT Manager инициализирован" ===
        // Успешная инициализация видна по последующему "[MQTT] Подключено"
        // или "[MQTT] FAIL ..." от MqttManager::reconnect().
    } else {
        Serial.println("[MQTT] Ошибка инициализации");
    }
    return result;
}

bool MqttService::begin(const String& server, int port,
                        const String& user, const String& password,
                        const String& stateTopic) {
    if (!configure(server, port, user, password, stateTopic)) {
        Serial.println("[MQTT] Ошибка сохранения настроек");
        return false;
    }
    return begin();
}

bool MqttService::configure(const String& server, int port,
                            const String& user, const String& password,
                            const String& stateTopic) {
    _credentials.setServer(server);
    _credentials.setPort(port);
    _credentials.setUser(user);
    _credentials.setPassword(password);
    _credentials.setStateTopic(stateTopic);
    return saveCredentials();
}

bool MqttService::saveCredentials() {
    return _credentials.save();
}

bool MqttService::loadCredentials() {
    return _credentials.load();
}

void MqttService::update() {
    _manager.update();
}

bool MqttService::isConnected() {
    return _manager.isConnected();
}

void MqttService::setMessageCallback(MqttMessageCallback callback) {
    _messageCallback = callback;
}

void MqttService::setOnConnected(MqttConnectedCallback callback) {
    _connectedCallback = callback;
}

bool MqttService::subscribeState() {
    String stateTopic = _credentials.getStateTopic();
    if (stateTopic.length() == 0) {
        Serial.println("[MQTT] state-топик не задан");
        return false;
    }
    return _manager.subscribe(stateTopic);
}

void MqttService::handleMessage(const String& topic, const String& payload) {
    // === ИЗМЕНЕНО: убран лог "MQTT получено: topic=..., payload=..." ===
    // Он дублировал то, что уже логирует LampStateService. Транспортный
    // уровень сам по себе не несёт полезной информации — важно только
    // применение значения, а это видно по "[LAMP] ...".
    if (_messageCallback) {
        _messageCallback(topic, payload);
    }
}