#include "storage.h"
#include "stats.h"
#include <Preferences.h>

static Preferences prefs;
static unsigned long lastSaveTime = 0;
static const unsigned long SAVE_INTERVAL = 60000; // Сохраняем раз в минуту

void storageInit() {
  // Открываем пространство имен "telemetry"
  prefs.begin("telemetry", false); 
  
  // Загружаем сохраненные значения (если их нет, вернется 0)
  g_lostPackets = prefs.getULong("lost", 0);
  g_retries     = prefs.getULong("retries", 0);
}

void storageUpdate() {
  // Периодически сохраняем текущие значения
  if (millis() - lastSaveTime >= SAVE_INTERVAL) {
    lastSaveTime = millis();
    prefs.putULong("lost", g_lostPackets);
    prefs.putULong("retries", g_retries);
  }
}

void storageReset() {
  g_lostPackets = 0;
  g_retries = 0;
  prefs.putULong("lost", 0);
  prefs.putULong("retries", 0);
}