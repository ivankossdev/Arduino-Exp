#include "LedManager.h"

LedManager::LedManager()
    : _pin(-1), _activeLow(true), _state(false), _mode(LED_OFF),
      _lastToggle(0), _interval(0), _ledOn(false) {}

bool LedManager::begin(int pin, bool activeLow) {
    if (pin < 0 || pin > 33) return false;
    _pin = pin;
    _activeLow = activeLow;
    pinMode(_pin, OUTPUT);
    _setOutput(false); // выключено по умолчанию
    _mode = LED_OFF;
    _state = false;
    return true;
}

void LedManager::_setOutput(bool on) {
    _ledOn = on;
    digitalWrite(_pin, _activeLow ? !on : on); // если activeLow, инвертируем
}

void LedManager::setMode(LedMode mode) {
    _mode = mode;
    _updateInterval();
    _lastToggle = millis();
    if (mode == LED_ON) {
        _setOutput(true);
        _state = true;
    } else if (mode == LED_OFF) {
        _setOutput(false);
        _state = false;
    } else {
        // для мигания начинаем с выключенного состояния
        _setOutput(false);
        _state = false;
    }
}

void LedManager::_updateInterval() {
    switch (_mode) {
        case LED_BLINK_SLOW:  _interval = 500; break;
        case LED_BLINK_FAST:  _interval = 100; break;
        case LED_BLINK_ERROR: _interval = 200; break;
        default:              _interval = 0; break;
    }
}

void LedManager::update() {
    if (_mode == LED_OFF || _mode == LED_ON || _interval == 0) return;

    unsigned long now = millis();
    if (now - _lastToggle >= _interval) {
        _lastToggle = now;
        if (_mode == LED_BLINK_ERROR) {
            // В режиме ошибки: короткая вспышка, затем пауза
            // Просто переключаем с интервалом, но пауза длиннее – сделаем асимметричный?
            // Для простоты делаем обычное переключение с периодом 200мс, но можно доработать
            _setOutput(!_ledOn);
            _state = _ledOn;
        } else {
            _setOutput(!_ledOn);
            _state = _ledOn;
        }
    }
}

void LedManager::on() {
    setMode(LED_ON);
}

void LedManager::off() {
    setMode(LED_OFF);
}

void LedManager::toggle() {
    _setOutput(!_ledOn);
    _state = _ledOn;
}

bool LedManager::getState() const {
    return _state;
}