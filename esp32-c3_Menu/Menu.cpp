#include "Menu.h"


Menu::Menu(int pin)
  : ledPin(pin), menuChoice(0) {}


void Menu::begin() {
  led.begin(ledPin);
  led.blink(); 
  preferences.begin("menu", false);  // Инициализация Preferences с пространством имён "menu"
  loadSettings();                    // Загрузка настроек из памяти
  applyLedState();                   // Применение настроик

  Serial.begin(115200);

  while (!Serial) {
     delay(100);
  }

  SerialBuferClear();
  printMenu();
}


void Menu::update() {
  if (Serial.available() > 0) {

    menuChoice = Serial.parseInt();
    handleMenuChoice(menuChoice);

    if (menuChoice != 0) {
      Serial.println("\nПоказать меню: 0");
    }

    SerialBuferClear();  // Очищаем буфер порта от мусора
  }
}


/* Очистка буфера COM порта */
void Menu::SerialBuferClear() {
  while (Serial.available()) {
    Serial.read();
  }
}


/* Меню для пользователя  */
void Menu::printMenu() {
  Serial.println("\n=== Интерактивное меню ESP32 ===");
  Serial.println("1. Включить светодиод");
  Serial.println("2. Выключить светодиод");
  Serial.println("3. Показать состояние");
  Serial.println("4. Сохранить настройки");
  Serial.println("5. Сброс на заводсике настройки");
  Serial.println("0. Показать меню снова");
}


void Menu::handleMenuChoice(int choice) {

  Serial.print("Выбрали: ");
  Serial.println(menuChoice);

  switch (choice) {
    case 1:
      led.on();
      Serial.println("Светодиод ВКЛЮЧЕН");
      break;
    case 2:
      led.off();
      Serial.println("Светодиод ВЫКЛЮЧЕН");
      break;
    case 3:
      Serial.print("Состояние светодиода: ");
      Serial.println(led.state ? "ВКЛЮЧЕН" : "ВЫКЛЮЧЕН");
      break;
    case 4:
      saveSettings();
      Serial.println("Настройки сохранены");
      break;
    case 5:
      led.state = false;
      saveSettings();
      applyLedState();
      Serial.println("Настройки сброшены");
      break;
    case 0:
      printMenu();
      break;
    default:
      Serial.println("Неверная опция! Выберите 0–5.");
      break;
  }
}


void Menu::saveSettings() {
  preferences.putBool("led_state", led.state);
}


void Menu::loadSettings() {
  led.state = preferences.getBool("led_state", false);
}


void Menu::applyLedState() {
  if (led.state) {
    led.on();
  } else {
    led.off();
  }
}
