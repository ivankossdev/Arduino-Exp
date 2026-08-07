#include "AppState.h"


AppState::AppState()
    : _state(AppStateEnum::IDLE), _networkCount(0), _hasScanResult(false) {}


void AppState::setState(AppStateEnum newState) {
    _state = newState;
    // Обновляем светодиод в зависимости от состояния
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

void AppState::autoConnect() {
    int count = _credentials.count();
    if (count == 0) {
        Serial.println("ℹ️ Нет сохранённых сетей для автоподключения.");
        setState(AppStateEnum::ERROR);
        return;
    }

    String ssid = _credentials.getSSID(0);
    String password = _credentials.getPasswordByIndex(0);

    if (ssid.length() == 0 || password.length() == 0) {
        Serial.println("⚠️ Ошибка получения данных первой сети.");
        setState(AppStateEnum::ERROR);
        return;
    }

    Serial.printf("🔄 Автоподключение к \"%s\"...\n", ssid.c_str());
    connect(ssid, password);
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
            Serial.print("Введите пароль для сети \"");
            Serial.print(currentSSID);
            Serial.print("\": ");
            String pass = Serial.readStringUntil('\n');
            pass.trim();
            if (pass.length() == 0) {
                Serial.println("❌ Пароль не может быть пустым, сохранение отменено.");
                return false;
            }
            _lastPassword = pass;
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

bool AppState::beginMqtt(const String& server, int port,
                         const String& user, const String& password,
                         const String& cmdTopic, const String& stateTopic) {
    bool result = _mqttManager.begin(server, port, user, password, cmdTopic, stateTopic);
    if (result) {
        // Устанавливаем колбэк для входящих сообщений
        MqttManager::setCallback([this](const String& topic, const String& payload) {
            this->handleMqttMessage(topic, payload);
        });
        Serial.println("MQTT Manager инициализирован");
    } else {
        Serial.println("Ошибка инициализации MQTT Manager");
    }
    return result;
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
    // Здесь можно реализовать логику управления, например, светодиодом
    // Для теста просто публикуем статус
    if (topic == "home/lamp/command") {  // в будущем можно сделать топики конфигурируемыми
        if (payload == "ON") {
            Serial.println("Lamp ON");
            _mqttManager.publishState("ON");
        } else if (payload == "OFF") {
            Serial.println("Lamp OFF");
            _mqttManager.publishState("OFF");
        }
    }
}








