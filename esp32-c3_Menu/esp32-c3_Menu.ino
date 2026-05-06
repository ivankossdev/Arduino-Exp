#include <Arduino.h>

#define LED_PIN 8
// Переменные для хранения состояния
int menuChoice = 0;
bool ledState = false;

/* Очистка буфера COM порта */
void SerialBuferClear() {
  while (Serial.available()) {
    Serial.read();
  }
}

/* Меню для пользователя  */
void printMenu() {
  Serial.println("\n=== Интерактивное меню ESP32 ===");
  Serial.println("1. Включить светодиод");
  Serial.println("2. Выключить светодиод");
  Serial.println("3. Показать состояние");
  Serial.println("0. Показать меню снова");
}

void handleMenuChoice(int choice) {

  Serial.print("Выбрали: ");
  Serial.println(menuChoice);

  switch (choice) {
    case 1:
      digitalWrite(LED_PIN, LOW);
      ledState = true;
      Serial.println("Светодиод ВКЛЮЧЕН");
      break;
    case 2:
      digitalWrite(LED_PIN, HIGH);
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

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(100);
  }

  SerialBuferClear();

  pinMode(8, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  delay(100);
  digitalWrite(LED_PIN, HIGH);
  delay(100);

  printMenu();
}

void loop() {
  if (Serial.available() > 0) {

    menuChoice = Serial.parseInt();
    handleMenuChoice(menuChoice);

    if(menuChoice != 0){
      Serial.println("\nПоказать меню: 0");
    }
    SerialBuferClear();  // Очищаем буфер порта от мусора
  }
  delay(100);  // Небольшая задержка для стабильности
}
