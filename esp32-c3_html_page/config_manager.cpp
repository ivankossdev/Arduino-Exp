#include "config_manager.h"
#include <Arduino.h>

bool ConfigManager::load() {
  if (!LittleFS.begin()) {
    Serial.println("❌ Не удалось инициализировать LittleFS");
    return false;
  }

  if (!LittleFS.exists("/config.json")) {
    Serial.println("❌ Файл config.json не найден!");
    return false;
  }

  File file = LittleFS.open("/config.json", "r");
  if (!file) {
    Serial.println("❌ Не удалось открыть config.json");
    return false;
  }

  // Определяем размер буфера — 1KB должно хватить
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print("❌ Ошибка парсинга JSON: ");
    Serial.println(error.f_str());
    return false;
  }

  // Читаем Wi-Fi
  _config.wifi.ssid = doc["wifi"]["ssid"].as<String>();
  _config.wifi.password = doc["wifi"]["password"].as<String>();

  // Прочие настройки
  _config.ledPin = doc["led_pin"] | 8;
  _config.defaultTemp = doc["default_temperature"] | 21;
  _config.tempMax = doc["temperature_max"] | 30;
  _config.tempMin = doc["temperature_min"] | 18;

  // MQTT
  auto mqtt = doc["mqtt"];
  _config.mqtt.enabled = mqtt["enabled"] | false;
  _config.mqtt.broker = mqtt["broker"].as<String>();
  _config.mqtt.port = mqtt["port"] | 1883;
  _config.mqtt.user = mqtt["user"].as<String>();
  _config.mqtt.password = mqtt["password"].as<String>();
  _config.mqtt.cmdTopic = mqtt["cmd_topic"].as<String>();
  _config.mqtt.stateTopic = mqtt["state_topic"].as<String>();

  Serial.println("✅ Конфигурация загружена из config.json");
  return true;
}
