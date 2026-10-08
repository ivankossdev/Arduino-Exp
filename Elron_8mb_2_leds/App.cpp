#include "App.h"
#include "EventTimer.h"
#include "Settings.h"
#include "Button.h"
#include "Console.h"

// ==================== ВНУТРЕННИЕ КОНСТАНТЫ ====================
namespace {

const uint8_t BTN_ACTIVE = HIGH;
const uint8_t LED2_PIN = 4;
const unsigned long HEARTBEAT_MS = 10000;
const unsigned long VERY_LONG_PRESS_MS = 5000;
const unsigned long SAVE_INTERVAL_MS = 60000;

// ==================== ОБЪЕКТЫ ====================
EventTimer blinkTimer(1000, 10);
EventTimer led2Timer(500, 1);
EventTimer heartbeatTimer(HEARTBEAT_MS, 1);
EventTimer saveTimer(SAVE_INTERVAL_MS, 1);
Button button(BTN_BUILTIN, BTN_ACTIVE, 50);
Console console;

// ==================== КОЛБЭКИ ТАЙМЕРОВ ====================

void onBlink(bool state, int count) {
  digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
  Serial.print(settings.mode == SLOW ? "SLOW " : "FAST ");
  Serial.print("Count ");
  Serial.println(count);
}

void onLed2Blink(bool state, int count) {
  (void)count;
  digitalWrite(LED2_PIN, state ? HIGH : LOW);
}

void onSeriesComplete() {
  Serial.println();
}

void onHeartbeat(bool state, int count) {
  (void)state;
  (void)count;

  unsigned long ms = millis();
  unsigned long sec = ms / 1000;
  unsigned long min = sec / 60;
  unsigned long hrs = min / 60;

  Serial.print("[HB] up=");
  Serial.print(hrs);
  Serial.print("h");
  Serial.print(min % 60);
  Serial.print("m");
  Serial.print(sec % 60);
  Serial.print("s");

  Serial.print(" boot=");
  Serial.print(settings.bootCount);
  Serial.print(" btn=");
  Serial.print(settings.buttonCount);

  Serial.print(" mode=");
  Serial.print(settings.mode == SLOW ? "SLOW" : settings.mode == FAST ? "FAST"
                                                                      : "OFF");

  // Serial.print(" led2=");
  // if (settings.led2Interval == 0) Serial.println("off");
  // else {
  //   Serial.print(settings.led2Interval);
  //   Serial.println("ms");
  // }

  Serial.print(" led2=");
  if (settings.led2Interval == 0) Serial.print("off");
  else {
    Serial.print(settings.led2Interval);
    Serial.print("ms");
  }

  Serial.print(" dirty=");
  Serial.println(settingsIsDirty() ? "Y" : "N");
}

void onSaveTick(bool state, int count) {
  (void)state;
  (void)count;
  settingsFlushIfDirty();  // тихо: пишет только если есть изменения
}

// ==================== ПРИМЕНЕНИЕ НАСТРОЕК ====================

void applySettings() {
  blinkTimer.setMaxIterations(settings.blinkCount);
  button.setLongPressMs(settings.longPressMs);

  switch (settings.mode) {
    case SLOW:
      blinkTimer.setInterval(settings.intervalSlow);
      blinkTimer.start();
      break;
    case FAST:
      blinkTimer.setInterval(settings.intervalFast);
      blinkTimer.start();
      break;
    case OFF:
      blinkTimer.stop();
      digitalWrite(LED_BUILTIN, LOW);
      break;
  }

  if (settings.led2Interval == 0) {
    led2Timer.stop();
    digitalWrite(LED2_PIN, LOW);
  } else {
    led2Timer.setInterval(settings.led2Interval);
    led2Timer.start();
  }
}

// ==================== ЛОГИКА РЕЖИМОВ ====================

void switchMode() {
  switch (settings.mode) {
    case SLOW: settings.mode = FAST; break;
    case FAST: settings.mode = OFF; break;
    case OFF: settings.mode = SLOW; break;
  }

  settingsMarkDirty();
  applySettings();

  Serial.print("Mode -> ");
  Serial.println(settings.mode == SLOW ? "SLOW" : settings.mode == FAST ? "FAST"
                                                                        : "OFF");
}

void resetMode() {
  settings.mode = SLOW;
  applySettings();
  blinkTimer.reset();
  digitalWrite(LED_BUILTIN, LOW);
  settingsSave();

  Serial.println("Mode -> RESET");
}

void factoryReset() {
  settingsFactoryReset();
  applySettings();
  blinkTimer.reset();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Mode -> FACTORY RESET");
}

// Колбэк для Console
void onSettingsChanged() {
  applySettings();
}

}  // namespace

// ==================== ПУБЛИЧНЫЙ ИНТЕРФЕЙС ====================

void appSetup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BTN_BUILTIN, INPUT);
  pinMode(LED2_PIN, OUTPUT);

  settingsInit();
  settingsIncBootCount();
  Serial.print("Boot #");
  Serial.println(settings.bootCount);

  delay(50);
  Serial.print("BTN idle reading: ");
  Serial.println(digitalRead(BTN_BUILTIN) ? "HIGH" : "LOW");

  // Таймеры
  blinkTimer.setCallback(onBlink);
  blinkTimer.setSeriesCompleteCallback(onSeriesComplete);
  led2Timer.setCallback(onLed2Blink);
  heartbeatTimer.setCallback(onHeartbeat);
  saveTimer.setCallback(onSaveTick);

  // Кнопка
  button.setPressStartCallback(settingsIncButtonCount);
  button.setShortPressCallback(switchMode);
  button.setLongPressCallback(resetMode);
  button.setVeryLongPressCallback(factoryReset);
  button.setVeryLongPressMs(VERY_LONG_PRESS_MS);

  // Консоль
  console.setOnSettingsChanged(onSettingsChanged);

  // Применяем восстановленный режим из EEPROM
  applySettings();

  Serial.print("Mode -> ");
  Serial.println(settings.mode == SLOW ? "SLOW" : settings.mode == FAST ? "FAST"
                                                                        : "OFF");

  console.printPrompt();
}

void appLoop() {
  blinkTimer.tick();
  led2Timer.tick();
  heartbeatTimer.tick();
  saveTimer.tick();
  button.tick();
  console.tick();
}