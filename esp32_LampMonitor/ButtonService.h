// ============================================================
// ButtonService — обработка двух кнопок TTGO T-Display v1.1
// ============================================================
// НОВЫЙ ФАЙЛ. Отвечает ТОЛЬКО за чтение кнопок и антидребезг.
// Никаких знаний о дисплее, настройках, Wi-Fi — только события.
//
// Кнопки:
//   BUTTON_SERVICE_PIN (35) — правая. Одиночное нажатие → onServicePressed.
//   BUTTON_RESET_PIN   (0)  — левая. Долгое удержание → onResetHoldStart,
//                              затем onResetHoldTick(secLeft) раз в секунду,
//                              затем onResetConfirmed при достижении порога
//                              или onResetHoldCancel при отпускании.
//
// Все решения о том, что делать с этими событиями, принимают
// подписчики (DisplayService, AppState).
// ============================================================

#ifndef BUTTON_SERVICE_H
#define BUTTON_SERVICE_H

#include <Arduino.h>
#include <functional>
#include "config.h"

typedef std::function<void()>    ButtonCallback;
typedef std::function<void(int)> ButtonTickCallback;

class ButtonService {
public:
    ButtonService();

    void begin();
    void update();

    // --- Подписка на события ---
    void setOnServicePressed(ButtonCallback cb)   { _onServicePressed = cb; }
    void setOnResetHoldStart(ButtonCallback cb)   { _onResetHoldStart = cb; }
    void setOnResetHoldTick(ButtonTickCallback cb){ _onResetHoldTick = cb; }
    void setOnResetHoldCancel(ButtonCallback cb)  { _onResetHoldCancel = cb; }
    void setOnResetConfirmed(ButtonCallback cb)   { _onResetConfirmed = cb; }

    // --- Состояние для внешних проверок ---
    bool isResetHolding() const { return _resetHoldActive; }
    bool isResetButtonPressed() const;

    // Блокирующее ожидание отпускания левой кнопки.
    // Используется перед ESP.restart(), чтобы не уйти в download mode
    // (GPIO0 — strapping-пин).
    void waitForResetRelease(unsigned long timeoutMs);

private:
    // Правая кнопка
    bool          _servicePressReported;
    bool          _lastServiceState;
    unsigned long _lastServiceChange;

    // Левая кнопка
    bool          _resetHoldActive;
    unsigned long _resetHoldStart;
    int           _lastResetSecondShown;
    bool          _lastResetState;
    unsigned long _lastResetChange;

    // Колбэки
    ButtonCallback     _onServicePressed;
    ButtonCallback     _onResetHoldStart;
    ButtonTickCallback _onResetHoldTick;
    ButtonCallback     _onResetHoldCancel;
    ButtonCallback     _onResetConfirmed;

    void updateServiceButton(unsigned long now);
    void updateResetButton(unsigned long now);
};

#endif