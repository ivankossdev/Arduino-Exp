#include "AppState.h"

AppState::AppState()
    : _stateManager(),
      _wifiService(_stateManager),
      _mqttService(_stateManager),
      _ledService(_mqttService),   // === НОВОЕ: LedService зависит от MqttService ===
      _displayService(_stateManager, _wifiService, _mqttService, *this),
      _statusBuilder(_stateManager, _wifiService, _mqttService, _ledService)  // === НОВОЕ ===
{
    // Регистрируем роутер входящих MQTT-сообщений. Сам роутер тонкий:
    // он делегирует в LedService и в будущем сможет делегировать в другие сервисы.
    _mqttService.setMessageCallback([this](const String& topic, const String& payload) {
        this->handleMqttMessage(topic, payload);
    });

    // === УБРАНО (шаг 3): лямбда onConnected с логикой retained-синхронизации ===
    // Теперь LedService сам регистрирует свой onConnected-колбэк в конструкторе.

    // === УБРАНО (шаг 2): _led.setMode(LED_OFF) ===
    // Начальное состояние светодиода выставляет LedService::begin().

    // === УБРАНО (шаг 3): поля _awaitingRetainedState / _connectedAt / _retainedWaitTimeout ===
    // Переехали в LedService.
}

void AppState::begin() {
    _wifiService.begin();
    _displayService.begin();
}

// === ИЗМЕНЕНО (шаг 1): делегат в StatusBuilder ===
String AppState::getStatusString() const {
    return _statusBuilder.buildText();
}

// --- Светодиод (делегаты в LedService) — шаг 2 ---

bool AppState::beginLed(int pin, bool activeLow) {
    return _ledService.begin(pin, activeLow);
}

void AppState::updateLed() {
    // Оставлено для совместимости. Основной путь обновления — AppState::update().
    _ledService.update();
}

void AppState::setLed(bool on) {
    // LedService::set сам публикует состояние в MQTT, если брокер подключён.
    _ledService.set(on);
}

bool AppState::getLedState() const {
    return _ledService.isOn();
}

// === ИЗМЕНЕНО (шаг 3): handleMqttMessage стал тонким роутером ===
void AppState::handleMqttMessage(const String& topic, const String& payload) {
    Serial.printf("📨 AppState: получено MQTT сообщение: топик=%s, payload=%s\n",
                  topic.c_str(), payload.c_str());

    // Делегируем в LedService. Он сам решит, относится ли сообщение к лампе
    // (retained-state из state-топика либо команда из cmd-топика).
    if (_ledService.handleMqttMessage(topic, payload)) return;

    // Сюда в будущем можно добавлять другие обработчики (reboot, status request и т.п.).
}

// === ИЗМЕНЕНО (шаги 2–3): update теперь дёргает LedService ===
void AppState::update() {
    // LedService::update() делает и _led.update(), и проверку retained-таймаута.
    _ledService.update();
    _mqttService.update();
    _displayService.update();
}

// === ИЗМЕНЕНО (шаг 1): делегат в StatusBuilder ===
String AppState::getStatusJson() {
    return _statusBuilder.buildJson();
}