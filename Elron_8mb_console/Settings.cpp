#include "Settings.h"
#include <EEPROM.h>

const int     EEPROM_ADDR_SETTINGS = 0;
const uint8_t SETTINGS_VERSION     = 1;

// Значения по умолчанию — в одном месте, используются и при init, и при reset
static const Settings DEFAULTS = {
  SLOW,    // mode
  1000,    // intervalSlow
  250,     // intervalFast
  10,      // blinkCount
  2000,    // longPressMs
  SETTINGS_VERSION
};

// Стартовый объект — копия DEFAULTS
Settings settings = {
  SLOW, 1000, 250, 10, 2000, SETTINGS_VERSION
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

  settings = loaded;
  Serial.println("[EEPROM] Settings loaded");
}

void settingsFactoryReset() {
  settings = DEFAULTS;
  settingsSave();
  Serial.println("[EEPROM] Factory reset");
}