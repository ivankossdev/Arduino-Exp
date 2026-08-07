#include "Menu.h"
#include "AppState.h"
#include "MqttManager.h"   // новый заголовок

AppState appState;
Menu menu(appState);
MqttManager mqttManager;   // глобальный объект

// Пример колбэка для обработки команд
void mqttCallback(const String& topic, const String& payload) {
    // Здесь можно реализовать логику управления, например, светодиодом
    Serial.printf("Обработка команды: %s -> %s\n", topic.c_str(), payload.c_str());

    // Пример: если пришла команда ON/OFF, управляем встроенным светодиодом (пин 2)
    // В будущем можно передавать в AppState
    if (topic == "home/lamp/command") {
        if (payload == "ON") {
            Serial.println("Lamp ON"); 
            mqttManager.publishState("ON");
        } else if (payload == "OFF") {
            Serial.println("Lamp OFF");
            mqttManager.publishState("OFF");
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(100);

    // Старая реализация 
    // Настройка встроенного светодиода (для демонстрации)

    // Инициализация AppState (автоподключение к Wi-Fi)
    appState.beginLed(8, true); // при необходимости
    appState.begin();
    menu.begin();

    // Инициализация MQTT сделано для теста 
    if (mqttManager.begin(
        "192.168.0.239",   // IP брокера
        1883,              // порт
        "esp32_user",      // логин
        "1234",            // пароль
        "home/lamp/command",  // командный топик
        "home/lamp/status",   // топик состояния
        mqttCallback          // колбэк на входящие сообщения
    )) {
        Serial.println("MQTT Manager инициализирован");
    } else {
        Serial.println("Ошибка инициализации MQTT Manager");
    }
}

void loop() {
    menu.update();
    appState.updateLed(); // если есть LedManager
    mqttManager.update(); // обработка MQTT
}