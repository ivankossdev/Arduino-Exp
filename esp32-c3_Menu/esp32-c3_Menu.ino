#include <Arduino.h>

// Переменные для хранения состояния
int menuChoice = 0;
bool ledState = false;
int pwmValue = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  pinMode(2, OUTPUT); // Встроенный светодиод
  printMenu();
}

void loop() {
  if (Serial.available() > 0) {
    menuChoice = Serial.parseInt();
    handleMenuChoice(menuChoice);
    printMenu(); // Показываем меню снова после выполнения
  }
  delay(100); // Небольшая задержка для стабильности
}

void printMenu() {
  Serial.println("\n=== Интерактивное меню ESP32 ===");
  Serial.println("1. Включить светодиод");
  Serial.println("2. Выключить светодиод");
  Serial.println("3. Управление ШИМ (введите значение 0–255)");
  Serial.println("4. Показать состояние");
  Serial.println("0. Показать меню снова");
  Serial.println("Выберите опцию: ");
}

void handleMenuChoice(int choice) {
  switch (choice) {
    case 1:
      digitalWrite(2, HIGH);
      ledState = true;
      Serial.println("Светодиод ВКЛЮЧЕН");
      break;
    case 2:
      digitalWrite(2, LOW);
      ledState = false;
      Serial.println("Светодиод ВЫКЛЮЧЕН");
      break;
    case 3:
      Serial.println("Введите значение ШИМ (0–255): ");
      while (!Serial.available()); // Ждём ввода
      pwmValue = Serial.parseInt();
      if (pwmValue >= 0 && pwmValue <= 255) {
        analogWrite(2, pwmValue);
        Serial.print("ШИМ установлен на: ");
        Serial.println(pwmValue);
      } else {
        Serial.println("Неверное значение! Должно быть 0–255.");
      }
      break;
    case 4:
      Serial.print("Состояние светодиода: ");
      Serial.println(ledState ? "ВКЛЮЧЕН" : "ВЫКЛЮЧЕН");
      Serial.print("Текущее значение ШИМ: ");
      Serial.println(pwmValue);
      break;
    case 0:
      // Просто показываем меню снова
      break;
    default:
      Serial.println("Неверная опция! Выберите 0–4.");
      break;
  }
}
