#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <Arduino.h>

enum class AppStateEnum {
    IDLE,
    SCANNING,
    CONNECTING,
    CONNECTED,
    AP_MODE,
    ERROR
};

class StateManager {
public:
    StateManager();

    // --- Управление состоянием ---
    void setState(AppStateEnum newState);
    AppStateEnum getState() const;

    // --- Проверки состояния ---
    bool isIdle() const;
    bool isScanning() const;
    bool isConnecting() const;
    bool isConnected() const;
    bool isError() const;
    bool isApMode() const;

    // --- Строковое представление ---
    const char* getStateString() const;

private:
    AppStateEnum _state;
};

#endif