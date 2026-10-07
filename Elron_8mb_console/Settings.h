#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

// Режимы работы устройства
enum Mode { SLOW, FAST, OFF };

// Профиль настроек, сохраняемый в EEPROM
struct __attribute__((packed)) Settings {
  uint8_t  mode;            // 0=SLOW, 1=FAST, 2=OFF
  uint32_t intervalSlow;    // мс
  uint32_t intervalFast;    // мс
  uint16_t blinkCount;      // тиков в серии
  uint16_t longPressMs;     // порог длинного нажатия
  uint8_t  version;         // версия структуры
};

extern Settings settings;   // глобальный объект настроек

void settingsInit();        // EEPROM.begin() + загрузка + валидация
void settingsSave();        // записать структуру в EEPROM
void settingsLoad();        // прочитать + проверить + применить
void settingsFactoryReset();// сброс к дефолтам

#endif // SETTINGS_H