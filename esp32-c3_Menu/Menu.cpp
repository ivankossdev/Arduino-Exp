#include "Menu.h"

Menu::Menu(int pin) : ledPin(pin), ledState(false), menuChoice(0) {}

void Menu::begin() {
  Serial.begin(115200);

  while (!Serial) {
    delay(100);
  }

  SerialBuferClear();
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);

  printMenu();
}

void Menu::update(){
  if (Serial.available() > 0) {

    menuChoice = Serial.parseInt();
    handleMenuChoice(menuChoice);

    if(menuChoice != 0){
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
    case 0:
      printMenu();
      break;
    default:
      Serial.println("Неверная опция! Выберите 0–3.");
      break;
  }
}