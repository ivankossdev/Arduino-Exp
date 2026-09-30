#include "LedService.h"

LedService::LedService(MqttService& mqttService)
    : _mqttService(mqttService)
{
    // === ШАГ 3: регистрируем себя как подписчика на (пере)подключение MQTT ===
    // Раньше эту лямбду ставил AppState в конструкторе. Теперь LedService
    // сам о себе заботится — AppState не знает про retained-синхронизацию.
    _mqttService.setOnConnected([this]() {
        this->onMqttConnected();
    });
}

bool LedService::begin(int pin, bool activeLow) {
    // === ШАГ 2: логика перенесена из AppState::beginLed ===
    bool ok = _led.begin(pin, activeLow);
    if (ok) {
        _led.setMode(LED_OFF);  // стартуем выключенным
    }
    return ok;
}

void LedService::update() {
    // === ШАГ 2: обновление мигания (перенесено из AppState::updateLed) ===
    _led.update();

    // === ШАГ 3: обработка таймаута retained (перенесено из AppState::update) ===
    // Если после (пере)подключения retained так и не пришёл (первый запуск,
    // пустой топик, state-топик не задан) — публикуем своё текущее состояние.
    if (_awaitingRetained && millis() - _connectedAt > _retainedTimeout) {
        _awaitingRetained = false;
        bool on = _led.getState();
        Serial.printf("⏱️ Retained-состояние не получено — публикуем своё: %s\n",
                      on ? "ON" : "OFF");
        _mqttService.publishState(on ? "ON" : "OFF");
    }
}

void LedService::set(bool on) {
    // === ШАГ 2: перенесено из AppState::setLed ===
    _led.setMode(on ? LED_ON : LED_OFF);

    // Публикуем новое состояние в MQTT, если брокер подключён.
    // Это делает веб-команды (WebService::handleLed -> AppState::setLed -> сюда)
    // видимыми на брокере.
    if (_mqttService.isConnected()) {
        _mqttService.publishState(on ? "ON" : "OFF");
    }
}

bool LedService::isOn() const {
    return _led.getState();
}

void LedService::onMqttConnected() {
    // === ШАГ 3: перенесено из лямбды AppState::AppState() ===
    // После (пере)подключения подписываемся на state-топик и ждём retained.
    // Если он придёт — применим его к лампе (см. handleMqttMessage).
    // Если нет — по таймауту опубликуем своё (см. update()).
    Serial.println("🔄 MQTT (пере)подключён — ожидаем retained-состояние из state-топика");
    _connectedAt = millis();
    _awaitingRetained = true;
    _mqttService.subscribeState();
}

bool LedService::handleMqttMessage(const String& topic, const String& payload) {
    // === ШАГ 3: топики берём из конфига, а не из хардкода ===
    // (это уже было сделано в предыдущей задаче, здесь сохраняем поведение)
    const String& stateTopic = _mqttService.getCredentials().getStateTopic();
    const String& cmdTopic   = _mqttService.getCredentials().getCmdTopic();

    // Retained-состояние после (пере)подключения: применяем к лампе без публикации.
    if (_awaitingRetained && topic == stateTopic) {
        _awaitingRetained = false;
        applyRetainedState(payload);
        return true;
    }

    // Команды из cmd-топика: ON / OFF.
    // set() внутри публикует подтверждение в state-топик.
    if (topic == cmdTopic) {
        if (payload == "ON") {
            set(true);
            return true;
        } else if (payload == "OFF") {
            set(false);
            return true;
        }
    }

    return false;
}

void LedService::applyRetainedState(const String& payload) {
    // Не публикуем обратно — иначе зациклимся.
    if (payload == "ON") {
        _led.setMode(LED_ON);
    } else if (payload == "OFF") {
        _led.setMode(LED_OFF);
    }
    Serial.printf("📥 Применено retained-состояние из MQTT: %s\n", payload.c_str());
}