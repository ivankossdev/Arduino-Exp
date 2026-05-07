#include "Menu.h"


Menu::Menu(int pin)
  : ledPin(pin), ledState(false), menuChoice(0) {}


void Menu::begin() {
  // Инициализация порта 
  pinMode(ledPin, OUTPUT);
  blink();
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


void Menu::blink(){
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
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
      digitalWrite(ledPin, LOW);
      ledState = true;
      Serial.println("Светодиод ВКЛЮЧЕН");
      break;
    case 2:
      digitalWrite(ledPin, HIGH);
      ledState = false;
      Serial.println("Светодиод ВЫКЛЮЧЕН");
      break;
    case 3:
      Serial.print("Состояние светодиода: ");
      Serial.println(ledState ? "ВКЛЮЧЕН" : "ВЫКЛЮЧЕН");
      break;
    case 4:
      saveSettings();
      Serial.println("Настройки сохранены");
      break;
    case 5:
      ledState = false;
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
  preferences.putBool("led_state", ledState);
}


void Menu::loadSettings() {
  ledState = preferences.getBool("led_state", false);
}


void Menu::applyLedState() {
  if (ledState) {
    digitalWrite(ledPin, LOW);  // Включить
  } else {
    digitalWrite(ledPin, HIGH); // Выключить
  }
}
