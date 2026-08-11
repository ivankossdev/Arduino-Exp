#include "AppState.h"

AppState::AppState()
    : _stateManager(),           // инициализация StateManager
      _wifiService(_stateManager, _led) // передаём StateManager в WiFiService
{}

void AppState::begin() {
    // Запускаем автоподключение через WiFiService
    // (у WiFiService есть метод autoConnect, но он приватный – вызовем через публичный метод begin?)
    // Можно сделать публичный метод WiFiService::begin(), но проще вызвать autoConnect() напрямую через дружественный класс?
    // Вместо этого добавим публичный метод в WiFiService – например, begin().
    // Но мы не хотим менять интерфейс WiFiService – сделаем так: вызовем приватный метод через другое публичное.
    // Я добавлю в WiFiService публичный метод begin(), который вызывает autoConnect().
    // Покажу это ниже в изменениях WiFiService.
    // Пока что оставим заглушку – мы добавим метод позже.
    // Для совместимости добавим в WiFiService метод begin() и вызовем его.
    _wifiService.begin();  // добавим этот метод в WiFiService (см. ниже)
}

String AppState::getStatusString() const {
    if (isConnected()) {
        String ssid = WiFi.SSID();
        IPAddress ip = WiFi.localIP();
        return "Подключено к \"" + ssid + "\", IP: " + ip.toString();
    } else {
        return "Не подключено к Wi-Fi.";
    }
}

// --- Светодиод ---
bool AppState::beginLed(int pin, bool activeLow) {
    return _led.begin(pin, activeLow);
}

void AppState::updateLed() {
    _led.update();
}

// --- MQTT ---
bool AppState::beginMqtt() {
    if (!loadMqttCredentials()) {
        Serial.println("⚠️ Нет сохранённых настроек MQTT.");
        return false;
    }

    String server = _mqttCredentials.getServer();
    int port = _mqttCredentials.getPort();
    String user = _mqttCredentials.getUser();
    String password = _mqttCredentials.getPassword();
    String cmdTopic = _mqttCredentials.getCmdTopic();
    String stateTopic = _mqttCredentials.getStateTopic();

    if (server.length() == 0) {
        Serial.println("⚠️ Сервер MQTT не задан.");
        return false;
    }

    bool result = _mqttManager.begin(server, port, user, password, cmdTopic, stateTopic);
    if (result) {
        MqttManager::setCallback([this](const String& topic, const String& payload) {
            this->handleMqttMessage(topic, payload);
        });
        Serial.println("MQTT Manager инициализирован с сохранёнными настройками");
    } else {
        Serial.println("Ошибка инициализации MQTT Manager");
    }
    return result;
}

bool AppState::beginMqtt(const String& server, int port,
                         const String& user, const String& password,
                         const String& cmdTopic, const String& stateTopic) {
    if (!configureMqtt(server, port, user, password, cmdTopic, stateTopic)) {
        Serial.println("Ошибка сохранения настроек MQTT");
        return false;
    }
    return beginMqtt();
}

bool AppState::configureMqtt(const String& server, int port,
                             const String& user, const String& password,
                             const String& cmdTopic, const String& stateTopic) {
    _mqttCredentials.setServer(server);
    _mqttCredentials.setPort(port);
    _mqttCredentials.setUser(user);
    _mqttCredentials.setPassword(password);
    _mqttCredentials.setCmdTopic(cmdTopic);
    _mqttCredentials.setStateTopic(stateTopic);
    return saveMqttCredentials();
}

bool AppState::saveMqttCredentials() {
    return _mqttCredentials.save();
}

bool AppState::loadMqttCredentials() {
    return _mqttCredentials.load();
}

void AppState::updateMqtt() {
    _mqttManager.update();
}

void AppState::update() {
    updateLed();
    updateMqtt();
}

void AppState::handleMqttMessage(const String& topic, const String& payload) {
    Serial.printf("MQTT получено: топик=%s, сообщение=%s\n", topic.c_str(), payload.c_str());
    if (topic == "home/lamp/command") {
        if (payload == "ON") {
            Serial.println("Lamp ON");
            _mqttManager.publishState("ON");
        } else if (payload == "OFF") {
            Serial.println("Lamp OFF");
            _mqttManager.publishState("OFF");
        }
    }
}