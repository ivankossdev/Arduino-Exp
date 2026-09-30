#include "MqttService.h"

MqttService::MqttService(StateManager& stateManager)
    : _stateManager(stateManager), _messageCallback(nullptr),
      _connectedCallback(nullptr) {}

bool MqttService::begin() {
    if (!loadCredentials()) {
        Serial.println("⚠️ Нет сохранённых настроек MQTT.");
        return false;
    }

    String server = _credentials.getServer();
    int port = _credentials.getPort();
    String user = _credentials.getUser();
    String password = _credentials.getPassword();
    String stateTopic = _credentials.getStateTopic();

    if (server.length() == 0) {
        Serial.println("⚠️ Сервер MQTT не задан.");
        return false;
    }

    bool result = _manager.begin(server, port, user, password, stateTopic);
    if (result) {
        MqttManager::setCallback([this](const String& topic, const String& payload) {
            this->handleMessage(topic, payload);
        });

        MqttManager::setOnConnected([this]() {
            if (this->_connectedCallback) {
                this->_connectedCallback();
            }
        });

        Serial.println("MQTT Manager инициализирован");
    } else {
        Serial.println("Ошибка инициализации MQTT Manager");
    }
    return result;
}

// === ИЗМЕНЕНО: сигнатура без cmdTopic ===
bool MqttService::begin(const String& server, int port,
                        const String& user, const String& password,
                        const String& stateTopic) {
    if (!configure(server, port, user, password, stateTopic)) {
        Serial.println("Ошибка сохранения настроек MQTT");
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
    // === УДАЛЕНО: setCmdTopic ===
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

// === УДАЛЕНО: publishState ===

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
        Serial.println("⚠️ MQTT: state-топик не задан, пропускаем подписку.");
        return false;
    }
    return _manager.subscribe(stateTopic);
}

void MqttService::handleMessage(const String& topic, const String& payload) {
    Serial.printf("MQTT получено: topic=%s, payload=%s\n", topic.c_str(), payload.c_str());

    if (_messageCallback) {
        _messageCallback(topic, payload);
    }
    // === УДАЛЕНО: fallback с хардкодом home/lamp/command ===
    // Теперь единственный путь — через _messageCallback → AppState → LampStateService.
}