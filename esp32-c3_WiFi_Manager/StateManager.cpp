#include "StateManager.h"

StateManager::StateManager() : _state(AppStateEnum::IDLE) {}

void StateManager::setState(AppStateEnum newState) {
    _state = newState;
    // Логирование изменения состояния (полезно для отладки)
    Serial.printf("📊 Состояние изменено: %s\n", getStateString());
}

AppStateEnum StateManager::getState() const {
    return _state;
}

bool StateManager::isIdle() const {
    return _state == AppStateEnum::IDLE;
}

bool StateManager::isScanning() const {
    return _state == AppStateEnum::SCANNING;
}

bool StateManager::isConnecting() const {
    return _state == AppStateEnum::CONNECTING;
}

bool StateManager::isConnected() const {
    return _state == AppStateEnum::CONNECTED;
}

bool StateManager::isError() const {
    return _state == AppStateEnum::ERROR;
}

bool StateManager::isApMode() const {
    return _state == AppStateEnum::AP_MODE;
}

const char* StateManager::getStateString() const {
    switch (_state) {
        case AppStateEnum::IDLE:       return "IDLE";
        case AppStateEnum::SCANNING:   return "SCANNING";
        case AppStateEnum::CONNECTING: return "CONNECTING";
        case AppStateEnum::CONNECTED:  return "CONNECTED";
        case AppStateEnum::AP_MODE:    return "AP_MODE";
        case AppStateEnum::ERROR:      return "ERROR";
        default:                       return "UNKNOWN";
    }
}