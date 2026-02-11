#include <Arduino.h>

// Ваша конфигурация
#define ENCODER_PIN_A D4   // GPIO2
#define ENCODER_PIN_B A0   // ADC0
#define ENCODER_BUTTON D0  // GPIO16

// Библиотека для энкодера (установить через менеджер библиотек)
#include <Encoder.h>
Encoder myEncoder(ENCODER_PIN_A, ENCODER_PIN_B);

void setup() {
  Serial.begin(115200);
  
  // Настройка пинов
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  pinMode(ENCODER_BUTTON, INPUT_PULLDOWN_16);
  
  Serial.println("Энкодер с библиотекой готов");
}

void loop() {
  static long oldPosition = 0;
  static bool oldButtonState = LOW;
  
  // Чтение энкодера
  long newPosition = myEncoder.read() / 4; // Деление для плавности
  
  if (newPosition != oldPosition) {
    Serial.print("Энкодер: ");
    Serial.println(newPosition);
    oldPosition = newPosition;
  }
  
  // Чтение кнопки
  bool buttonState = digitalRead(ENCODER_BUTTON);
  
  if (buttonState != oldButtonState) {
    delay(50); // Дебаунс
    if (buttonState == HIGH) { // HIGH = нажата для INPUT_PULLDOWN_16
      Serial.println("Кнопка!");
      // myEncoder.write(0); // Сброс при нажатии
    }
    oldButtonState = buttonState;
  }
  
  delay(10);
}