#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <Arduino.h>
#include <functional>

enum class AppStateEnum {
    IDLE,
    SCANNING,
    CONNECTING,
    CONNECTED,
    AP_MODE,
    ERROR
};

// Тип колбэка для уведомления об изменении состояния
typedef std::function<void(AppStateEnum newState)> StateChangeCallback;

class StateManager {
public:
    StateManager();

    // Установка состояния
    void setState(AppStateEnum newState);
    AppStateEnum getState() const;

    // Проверки состояния
    bool isIdle() const;
    bool isScanning() const;
    bool isConnecting() const;
    bool isConnected() const;
    bool isError() const;
    bool isApMode() const;

    // Строковое представление
    const char* getStateString() const;

    // Регистрация колбэка на изменение состояния
    void setOnStateChange(StateChangeCallback callback);

private:
    AppStateEnum _state;
    StateChangeCallback _onChange;
};

#endif