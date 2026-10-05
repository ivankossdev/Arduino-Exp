// ============================================================
// AppState — координатор сервисов
// ============================================================
// ИЗМЕНЕНО: добавлен ButtonService в список членов.
// ИЗМЕНЕНО: здесь же связаны колбэки кнопок с DisplayService
//           и performFactoryReset().
// ИЗМЕНЕНО: performFactoryReset() перенесён сюда из DisplayService.
// ============================================================

#include "AppState.h"
#include "log.h"                 // === НОВОЕ: для логов в performFactoryReset

AppState::AppState()
    : _stateManager(),
      _wifiService(_stateManager),
      _mqttService(_stateManager),
      _lampStateService(_mqttService),
      _buttonService(),                                   // === НОВОЕ
      _displayService(_stateManager, _wifiService, _mqttService, _lampStateService),
      _statusBuilder(_stateManager, _wifiService, _mqttService, _lampStateService)
{
    // === MQTT-колбэк (как было) ===
    _mqttService.setMessageCallback([this](const String& topic, const String& payload) {
        this->handleMqttMessage(topic, payload);
    });

    // === НОВОЕ: связываем ButtonService с DisplayService и AppState ===
    // DisplayService сам решает, реагировать ли на нажатие правой кнопки
    // (проверяет своё состояние и текущий вид).
    _buttonService.setOnServicePressed([this]() {
        _displayService.onServiceButtonPressed();
    });

    _buttonService.setOnResetHoldStart([this]() {
        _displayService.onResetHoldStart();
    });

    _buttonService.setOnResetHoldTick([this](int secLeft) {
        _displayService.onResetHoldTick(secLeft);
    });

    _buttonService.setOnResetHoldCancel([this]() {
        _displayService.onResetHoldCancel();
    });

    _buttonService.setOnResetConfirmed([this]() {
        this->performFactoryReset();
    });
}

void AppState::begin() {
    _buttonService.begin();     // === НОВОЕ: инициализация пинов
    _wifiService.begin();
    _displayService.begin();
}

String AppState::getStatusString() const {
    return _statusBuilder.buildText();
}

void AppState::handleMqttMessage(const String& topic, const String& payload) {
    if (_lampStateService.handleMqttMessage(topic, payload)) return;
}

void AppState::update() {
    // === НОВОЕ: опрос кнопок. Идёт первым, чтобы события успели
    // сработать до обновления экрана.
    _buttonService.update();

    _mqttService.update();
    _displayService.update();
}

String AppState::getStatusJson() {
    return _statusBuilder.buildJson();
}

// ============================================================
// Фабричный сброс
// ============================================================
// Вызывается по onResetConfirmed от ButtonService (левая кнопка
// удерживалась BUTTON_RESET_HOLD_MS).
//
// Порядок:
//   1. Показать "RESETTING" (визуальная отдача).
//   2. Стереть Wi-Fi credentials.
//   3. Сбросить MQTT-настройки на дефолтные.
//   4. Дождаться отпускания левой кнопки (GPIO0 strapping —
//      иначе уйдём в download mode).
//   5. ESP.restart().
void AppState::performFactoryReset() {
    LOG_LN(LOG_CAT_SYS, "[SYS] Factory reset triggered");

    _displayService.showResettingScreen();

    while (_wifiService.getSavedCount() > 0) {
        _wifiService.deleteSavedNetwork(0);
    }

    _mqttService.configure("", MQTT_DEFAULT_PORT, "", "", MQTT_DEFAULT_STATE_TOPIC);

    _buttonService.waitForResetRelease(BUTTON_RESET_RELEASE_WAIT_MS);

    delay(300);
    ESP.restart();
}