#include "Settings.h"
#include <EEPROM.h>

const int     EEPROM_ADDR_SETTINGS = 0;
const uint8_t SETTINGS_VERSION     = 3;

static const Settings DEFAULTS = {
  SLOW,    // mode
  1000,    // intervalSlow
  250,     // intervalFast
  10,      // blinkCount
  2000,    // longPressMs
  SETTINGS_VERSION,
  0,       // bootCount
  0,       // buttonCount
  0        // led2Interval = 0 (выключен)
};

Settings settings = {
  SLOW, 1000, 250, 10, 2000, SETTINGS_VERSION, 0, 0, 0
};

void settingsInit() {
  EEPROM.begin();
  settingsLoad();
}

void settingsSave() {
  EEPROM.put(EEPROM_ADDR_SETTINGS, settings);
}

void settingsLoad() {
  Settings loaded;
  EEPROM.get(EEPROM_ADDR_SETTINGS, loaded);

  // Если версия не совпала — берём дефолты
  if (loaded.version != SETTINGS_VERSION) {
    Serial.println("[EEPROM] Invalid version, using defaults");
    settings = DEFAULTS;
    return;
  }

  // Тихая валидация: подчиняем подозрительные значения
  if (loaded.mode > OFF)            loaded.mode = SLOW;
  if (loaded.intervalSlow < 50)     loaded.intervalSlow = 1000;
  if (loaded.intervalFast < 50)     loaded.intervalFast = 250;
  if (loaded.blinkCount == 0)       loaded.blinkCount = 10;
  if (loaded.longPressMs < 200)     loaded.longPressMs = 2000;
  if (loaded.led2Interval != 0 && loaded.led2Interval < 50) loaded.led2Interval = 0;   // слишком малый период — считаем «выключен»

  settings = loaded;
  Serial.println("[EEPROM] Settings loaded");
}

void settingsFactoryReset() {
  settings = DEFAULTS;
  settingsSave();
  Serial.println("[EEPROM] Factory reset");
}

void settingsIncBootCount() {
  settings.bootCount++;
  settingsSave();
}

void settingsIncButtonCount() {
  settings.buttonCount++;
  settingsSave();
}