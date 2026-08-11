#include "MqttService.h"

MqttService::MqttService(StateManager& stateManager)
    : _stateManager(stateManager), _messageCallback(nullptr) {}

// --- Инициализация с сохранёнными настройками ---
bool MqttService::begin() {
    if (!loadCredentials()) {
        Serial.println("⚠️ Нет сохранённых настроек MQTT.");
        return false;
    }

    String server = _credentials.getServer();
    int port = _credentials.getPort();
    String user = _credentials.getUser();
    String password = _credentials.getPassword();
    String cmdTopic = _credentials.getCmdTopic();
    String stateTopic = _credentials.getStateTopic();

    if (server.length() == 0) {
        Serial.println("⚠️ Сервер MQTT не задан.");
        return false;
    }

    bool result = _manager.begin(server, port, user, password, cmdTopic, stateTopic);
    if (result) {
        // Устанавливаем колбэк для входящих сообщений
        MqttManager::setCallback([this](const String& topic, const String& payload) {
            this->handleMessage(topic, payload);
        });
        Serial.println("MQTT Manager инициализирован с сохранёнными настройками");
    } else {
        Serial.println("Ошибка инициализации MQTT Manager");
    }
    return result;
}

// --- Инициализация с параметрами (сохраняет их) ---
bool MqttService::begin(const String& server, int port,
                        const String& user, const String& password,
                        const String& cmdTopic, const String& stateTopic) {
    if (!configure(server, port, user, password, cmdTopic, stateTopic)) {
        Serial.println("Ошибка сохранения настроек MQTT");
        return false;
    }
    return begin();
}

// --- Сохранение настроек в объект и в Preferences ---
bool MqttService::configure(const String& server, int port,
                            const String& user, const String& password,
                            const String& cmdTopic, const String& stateTopic) {
    _credentials.setServer(server);
    _credentials.setPort(port);
    _credentials.setUser(user);
    _credentials.setPassword(password);
    _credentials.setCmdTopic(cmdTopic);
    _credentials.setStateTopic(stateTopic);
    return saveCredentials();
}

bool MqttService::saveCredentials() {
    return _credentials.save();
}

bool MqttService::loadCredentials() {
    return _credentials.load();
}

// --- Обновление (вызывается из loop) ---
void MqttService::update() {
    _manager.update();
}

// --- Публикация ---
bool MqttService::publishState(const String& message, bool retained) {
    return _manager.publishState(message, retained);
}

// --- Статус ---
bool MqttService::isConnected() {
    return _manager.isConnected();
}

// --- Установка внешнего колбэка ---
void MqttService::setMessageCallback(MqttMessageCallback callback) {
    _messageCallback = callback;
}

// --- Приватный обработчик входящих сообщений ---
void MqttService::handleMessage(const String& topic, const String& payload) {
    Serial.printf("MQTT получено: топик=%s, сообщение=%s\n", topic.c_str(), payload.c_str());

    // Если зарегистрирован внешний колбэк – вызываем его
    if (_messageCallback) {
        _messageCallback(topic, payload);
    } else {
        // Встроенная обработка (для совместимости со старой логикой)
        if (topic == "home/lamp/command") {
            if (payload == "ON") {
                Serial.println("Lamp ON");
                publishState("ON");
            } else if (payload == "OFF") {
                Serial.println("Lamp OFF");
                publishState("OFF");
            }
        }
    }
}