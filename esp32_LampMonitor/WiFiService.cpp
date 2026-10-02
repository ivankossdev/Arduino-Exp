// ============================================================
// WiFiService — реализация для ESP32
// ============================================================
// ИЗМЕНЕНО: Serial.print* заменены на LOG/LOG_LN(LOG_CAT_WIFI)
// ============================================================

#include "WiFiService.h"
#include "log.h"       // ИЗМЕНЕНО: макросы логирования

// === ВАЖНО: конструктор ===
// Этот блок был пропущен в предыдущем ответе — отсюда ошибка
// "undefined reference to WiFiService::WiFiService(StateManager&)".
// Конструктор инициализирует ссылку на StateManager и счётчики.
WiFiService::WiFiService(StateManager& stateManager)
    : _stateManager(stateManager), _networkCount(0), _hasScanResult(false) {}

void WiFiService::setState(AppStateEnum newState) {
    _stateManager.setState(newState);
}

void WiFiService::begin() {
    autoConnect();
}

void WiFiService::setApCredentials(const String& ssid, const String& password) {
    _apSsid = ssid;
    _apPassword = password;
}

bool WiFiService::startScan() {
    if (_stateManager.getState() == AppStateEnum::SCANNING) return false;

    setState(AppStateEnum::SCANNING);
    int count = _wifiManager.scan(_networks, MAX_NETWORKS);

    if (count > 0) {
        _networkCount = count;
        _hasScanResult = true;
        setState(AppStateEnum::IDLE);
        return true;
    }
    _networkCount = 0;
    _hasScanResult = false;
    setState(AppStateEnum::IDLE);
    return false;
}

int WiFiService::getNetworkCount() const { return _networkCount; }

NetworkInfo WiFiService::getNetwork(int index) const {
    if (index >= 0 && index < _networkCount) return _networks[index];
    NetworkInfo empty;
    memset(&empty, 0, sizeof(empty));
    return empty;
}

bool WiFiService::hasScanResult() const { return _hasScanResult; }

bool WiFiService::connect(const String& ssid, const String& password) {
    setState(AppStateEnum::CONNECTING);
    bool success = _wifiManager.connectToNetwork(ssid.c_str(), password.c_str());
    if (success) {
        setState(AppStateEnum::CONNECTED);
        _lastSSID = ssid;
        _lastPassword = password;
    } else {
        setState(AppStateEnum::ERROR);
    }
    return success;
}

bool WiFiService::connectToNetwork(const String& ssid, const String& password) {
    if (ssid.length() == 0 || password.length() == 0) return false;
    return connect(ssid, password);
}

bool WiFiService::connectToSavedNetwork(int index) {
    String ssid = _credentials.getSSID(index);
    String password = _credentials.getPasswordByIndex(index);
    if (ssid.length() == 0 || password.length() == 0) return false;
    return connect(ssid, password);
}

bool WiFiService::saveCurrentNetwork() {
    if (!isConnected()) {
        return false;
    }
    String currentSSID = WiFi.SSID();
    if (currentSSID.length() == 0) {
        return false;
    }
    if (_lastSSID != currentSSID) {
        _lastSSID = currentSSID;
        String savedPass = _credentials.getPassword(currentSSID);
        if (savedPass.length() > 0) {
            _lastPassword = savedPass;
        } else {
            return false;
        }
    }
    if (_lastSSID.length() == 0 || _lastPassword.length() == 0) {
        return false;
    }
    if (_credentials.save(_lastSSID, _lastPassword)) {
        LOG(LOG_CAT_WIFI, "[WiFi] Сеть \"%s\" сохранена\n", _lastSSID.c_str());
        return true;
    }
    return false;
}

bool WiFiService::deleteSavedNetwork(int index) {
    String ssid = _credentials.getSSID(index);
    if (ssid.length() == 0) return false;
    return _credentials.remove(ssid);
}

int WiFiService::getSavedCount() { return _credentials.count(); }

String WiFiService::getSavedSSID(int index) { return _credentials.getSSID(index); }

String WiFiService::getSavedPassword(const String& ssid) {
    return _credentials.getPassword(ssid);
}

bool WiFiService::hasSavedPassword(const String& ssid) {
    return _credentials.hasCredentials(ssid);
}

void WiFiService::printSavedNetworks() { _credentials.printAll(); }

bool WiFiService::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiService::getCurrentSSID() const {
    if (isConnected()) return WiFi.SSID();
    return "";
}

IPAddress WiFiService::getIP() const {
    if (isConnected()) return WiFi.localIP();
    return IPAddress();
}

const char* WiFiService::getEncryptionType(uint8_t encType) const {
    return _wifiManager.getEncryptionType(encType);
}

void WiFiService::autoConnect() {
    int count = _credentials.count();
    if (count == 0) {
        LOG_LN(LOG_CAT_WIFI, "[WiFi] Нет сохранённых сетей");
        setState(AppStateEnum::ERROR);
        return;
    }

    for (int i = 0; i < count; i++) {
        String ssid = _credentials.getSSID(i);
        String password = _credentials.getPasswordByIndex(i);

        if (ssid.length() == 0 || password.length() == 0) continue;

        if (connect(ssid, password)) return;
        setState(AppStateEnum::IDLE);
        delay(WIFI_AUTOCONNECT_RETRY_DELAY_MS);
    }

    LOG_LN(LOG_CAT_WIFI, "[WiFi] Все попытки подключения провалились");
    setState(AppStateEnum::ERROR);
}