#include "AppState.h"

AppState::AppState()
    : _state(AppStateEnum::IDLE), _networkCount(0), _hasScanResult(false) {}

void AppState::setState(AppStateEnum newState) {
    _state = newState;
    switch (_state) {
        case AppStateEnum::IDLE:
            _led.setMode(LED_OFF);
            break;
        case AppStateEnum::SCANNING:
            _led.setMode(LED_BLINK_FAST);
            break;
        case AppStateEnum::CONNECTING:
            _led.setMode(LED_BLINK_SLOW);
            break;
        case AppStateEnum::CONNECTED:
            _led.setMode(LED_ON);
            break;
        case AppStateEnum::ERROR:
            _led.setMode(LED_BLINK_ERROR);
            break;
        default:
            _led.setMode(LED_OFF);
            break;
    }
}

void AppState::begin() {
    autoConnect();
}

// ИСПРАВЛЕННЫЙ МЕТОД АВТОПОДКЛЮЧЕНИЯ
void AppState::autoConnect() {
    int count = _credentials.count();
    if (count == 0) {
        Serial.println("ℹ️ Нет сохранённых сетей для автоподключения.");
        setState(AppStateEnum::ERROR);
        return;
    }

    for (int i = 0; i < count; i++) {
        String ssid = _credentials.getSSID(i);
        String password = _credentials.getPasswordByIndex(i);

        if (ssid.length() == 0 || password.length() == 0) {
            Serial.printf("⚠️ Ошибка получения данных сети #%d, пропускаем.\n", i);
            continue;
        }

        Serial.printf("🔄 Попытка подключения к \"%s\"...\n", ssid.c_str());
        if (connect(ssid, password)) {
            return; // успех – выходим
        }
        Serial.printf("❌ Не удалось подключиться к \"%s\", пробуем следующую.\n", ssid.c_str());
        setState(AppStateEnum::IDLE);
        delay(500);
    }

    Serial.println("❌ Не удалось подключиться ни к одной сохранённой сети.");
    setState(AppStateEnum::ERROR);
}

bool AppState::connect(const String& ssid, const String& password) {
    setState(AppStateEnum::CONNECTING);
    bool success = _wifiManager.connectToNetwork(ssid.c_str(), password.c_str());
    if (success) {
        setState(AppStateEnum::CONNECTED);
        _lastSSID = ssid;
        _lastPassword = password;
        Serial.println("✅ Подключено успешно!");
        Serial.print("IP-адрес: ");
        Serial.println(WiFi.localIP());
    } else {
        setState(AppStateEnum::ERROR);
        Serial.println("❌ Ошибка подключения.");
    }
    return success;
}

bool AppState::startScan() {
    if (_state == AppStateEnum::SCANNING) return false;
    setState(AppStateEnum::SCANNING);
    int count = _wifiManager.scan(_networks, MAX_NETWORKS);
    if (count > 0) {
        _networkCount = count;
        _hasScanResult = true;
        setState(AppStateEnum::IDLE);
        return true;
    } else {
        _networkCount = 0;
        _hasScanResult = false;
        setState(AppStateEnum::IDLE);
        return false;
    }
}

int AppState::getNetworkCount() const {
    return _networkCount;
}

NetworkInfo AppState::getNetwork(int index) const {
    if (index >= 0 && index < _networkCount) {
        return _networks[index];
    }
    NetworkInfo empty;
    memset(&empty, 0, sizeof(empty));
    return empty;
}

bool AppState::hasScanResult() const {
    return _hasScanResult;
}

bool AppState::connectToNetwork(const String& ssid, const String& password) {
    if (ssid.length() == 0 || password.length() == 0) return false;
    return connect(ssid, password);
}

bool AppState::connectToSavedNetwork(int index) {
    String ssid = _credentials.getSSID(index);
    String password = _credentials.getPasswordByIndex(index);
    if (ssid.length() == 0 || password.length() == 0) return false;
    return connect(ssid, password);
}

bool AppState::saveCurrentNetwork() {
    if (!isConnected()) {
        Serial.println("⚠️ Нет подключения к Wi-Fi.");
        return false;
    }
    String currentSSID = WiFi.SSID();
    if (currentSSID.length() == 0) {
        Serial.println("⚠️ Не удалось получить имя текущей сети.");
        return false;
    }
    if (_lastSSID != currentSSID) {
        _lastSSID = currentSSID;
        String savedPass = _credentials.getPassword(currentSSID);
        if (savedPass.length() > 0) {
            _lastPassword = savedPass;
        } else {
            Serial.println("⚠️ Пароль для этой сети не сохранён. Используйте меню для подключения.");
            return false;
        }
    }
    if (_lastSSID.length() == 0 || _lastPassword.length() == 0) {
        Serial.println("⚠️ Ошибка: нет данных для сохранения.");
        return false;
    }
    if (_credentials.save(_lastSSID, _lastPassword)) {
        Serial.printf("✅ Сеть \"%s\" сохранена.\n", _lastSSID.c_str());
        return true;
    } else {
        Serial.println("❌ Ошибка сохранения.");
        return false;
    }
}

bool AppState::deleteSavedNetwork(int index) {
    String ssid = _credentials.getSSID(index);
    if (ssid.length() == 0) return false;
    return _credentials.remove(ssid);
}

int AppState::getSavedCount() {
    return _credentials.count();
}

String AppState::getSavedSSID(int index) {
    return _credentials.getSSID(index);
}

String AppState::getSavedPassword(const String& ssid) {
    return _credentials.getPassword(ssid);
}

bool AppState::hasSavedPassword(const String& ssid) {
    return _credentials.hasCredentials(ssid);
}

void AppState::printSavedNetworks() {
    _credentials.printAll();
}

AppStateEnum AppState::getState() const {
    return _state;
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

bool AppState::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String AppState::getCurrentSSID() const {
    if (isConnected()) return WiFi.SSID();
    return "";
}

IPAddress AppState::getIP() const {
    if (isConnected()) return WiFi.localIP();
    return IPAddress();
}

const char* AppState::getEncryptionType(uint8_t encType) const {
    return _wifiManager.getEncryptionType(encType);
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