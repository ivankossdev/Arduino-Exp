// ============================================================
// ButtonService — реализация
// ============================================================
// НОВЫЙ ФАЙЛ. Логика перенесена из DisplayService без изменений
// по существу — только вместо draw-вызовов теперь колбэки.
// ============================================================

#include "ButtonService.h"
#include "log.h"

ButtonService::ButtonService()
    : _servicePressReported(false),
      _lastServiceState(false),
      _lastServiceChange(0),
      _resetHoldActive(false),
      _resetHoldStart(0),
      _lastResetSecondShown(-1),
      _lastResetState(false),
      _lastResetChange(0)
{}

void ButtonService::begin() {
    // Правая кнопка — GPIO35. Внешний pull-up на плате.
    pinMode(BUTTON_SERVICE_PIN, INPUT);
    _lastServiceState = (digitalRead(BUTTON_SERVICE_PIN) == LOW);
    _lastServiceChange = millis();

    // Левая кнопка — GPIO0. Внутренний pull-up, нажатие = LOW.
    pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
    _lastResetState = (digitalRead(BUTTON_RESET_PIN) == LOW);
    _lastResetChange = millis();
}

void ButtonService::update() {
    unsigned long now = millis();
    updateServiceButton(now);
    updateResetButton(now);
}

// ============================================================
// Правая кнопка — одиночное нажатие
// ============================================================
// Событие _onServicePressed срабатывает один раз на нажатие.
// Не «перезапускается», пока кнопку не отпустят и не нажмут снова.
void ButtonService::updateServiceButton(unsigned long now) {
    bool raw = (digitalRead(BUTTON_SERVICE_PIN) == LOW);

    if (raw != _lastServiceState) {
        _lastServiceChange = now;
        _lastServiceState = raw;
        return;   // идёт смена состояния — ждём антидребезг
    }

    // Стабильное нажатие после антидребезга
    if (raw && !_servicePressReported &&
        (now - _lastServiceChange) >= BUTTON_DEBOUNCE_MS) {
        _servicePressReported = true;
        if (_onServicePressed) _onServicePressed();
    }

    // Отпустили — сбрасываем флаг, чтобы следующее нажатие снова сработало
    if (!raw) {
        _servicePressReported = false;
    }
}

// ============================================================
// Левая кнопка — удержание для сброса
// ============================================================
void ButtonService::updateResetButton(unsigned long now) {
    bool raw = (digitalRead(BUTTON_RESET_PIN) == LOW);

    if (raw != _lastResetState) {
        _lastResetChange = now;
        _lastResetState = raw;
    }

    if (!_resetHoldActive) {
        if (raw && (now - _lastResetChange) >= BUTTON_DEBOUNCE_MS) {
            _resetHoldActive = true;
            _resetHoldStart = now;
            _lastResetSecondShown = -1;
            if (_onResetHoldStart) _onResetHoldStart();
        }
        return;
    }

    // Удержание активно. Отпустили до порога — отмена.
    if (!raw && (now - _lastResetChange) >= BUTTON_DEBOUNCE_MS) {
        _resetHoldActive = false;
        if (_onResetHoldCancel) _onResetHoldCancel();
        return;
    }

    unsigned long heldMs = now - _resetHoldStart;

    if (heldMs >= BUTTON_RESET_HOLD_MS) {
        // Порог достигнут. Флаг _resetHoldActive оставляем как есть —
        // AppState выполнит сброс и сам решит, когда сбросить флаг.
        if (_onResetConfirmed) _onResetConfirmed();
        return;
    }

    int secLeft = (BUTTON_RESET_HOLD_MS - heldMs + 999) / 1000;
    if (secLeft != _lastResetSecondShown) {
        _lastResetSecondShown = secLeft;
        if (_onResetHoldTick) _onResetHoldTick(secLeft);
    }
}

bool ButtonService::isResetButtonPressed() const {
    return digitalRead(BUTTON_RESET_PIN) == LOW;
}

void ButtonService::waitForResetRelease(unsigned long timeoutMs) {
    unsigned long start = millis();
    while (isResetButtonPressed() && millis() - start < timeoutMs) {
        delay(50);
    }
}