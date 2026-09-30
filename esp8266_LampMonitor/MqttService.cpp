#include "MqttService.h"

MqttService::MqttService(StateManager& stateManager)
    : _stateManager(stateManager), _messageCallback(nullptr),
      _connectedCallback(nullptr), _callbacksRegistered(false) {}

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

    // === ИСПРАВЛЕНИЕ: если уже подключены к старому брокеру — рвём соединение,
    // чтобы не остались висеть старые подписки. Без этого повторный вызов
    // begin() после сохранения настроек через веб мог приводить к дублированию
    // сообщений (SUBSCRIBE отправлялся дважды для одного и того же топика).
    _manager.disconnect();

    bool result = _manager.begin(server, port, user, password, stateTopic);
    if (result) {
        // === ИСПРАВЛЕНИЕ: регистрируем статические колбэки РОВНО ОДИН РАЗ ===
        // Если делать это при каждом begin(), вторая регистрация перезапишет
        // лямбду от первой. Само по себе это не смертельно, но исторически
        // провоцировало двойную подписку на брокере. Регистрируем один раз.
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

        Serial.println("MQTT Manager инициализирован");
    } else {
        Serial.println("Ошибка инициализации MQTT Manager");
    }
    return result;
}

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
}