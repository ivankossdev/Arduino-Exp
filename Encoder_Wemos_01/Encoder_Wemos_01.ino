#include <Arduino.h>

// Ваша конфигурация
#define ENCODER_PIN_A D4   // GPIO2 - будет мигать LED при вращении
#define ENCODER_PIN_B A0   // ADC0 - работает как цифровой пин
#define ENCODER_BUTTON D0  // GPIO16 - безопасная кнопка

// Для ручной обработки энкодера
volatile int encoderPos = 0;
int lastReportedPos = 0;
bool lastStateA = HIGH;

// Для кнопки
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Обработчик прерывания для энкодера
ICACHE_RAM_ATTR void handleEncoder() {
  bool stateA = digitalRead(ENCODER_PIN_A);
  bool stateB = digitalRead(ENCODER_PIN_B);
  
  if (stateA != lastStateA) {
    // Определяем направление вращения
    if (stateB != stateA) {
      encoderPos++;  // По часовой
    } else {
      encoderPos--;  // Против часовой
    }
  }
  lastStateA = stateA;
}

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n=== Энкодер на Wemos D1 ===");
  Serial.println("Конфигурация:");
  Serial.println("  S1  -> D4 (GPIO2)");
  Serial.println("  S2  -> A0 (ADC0)");
  Serial.println("  KEY -> D0 (GPIO16)");
  Serial.println("==========================\n");
  
  // Настройка пинов
  
  // D4 (GPIO2) - INPUT_PULLUP (встроенный LED будет работать)
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  
  // A0 (ADC0) - как цифровой INPUT_PULLUP
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  
  // D0 (GPIO16) - особый пин, только INPUT_PULLDOWN_16
  pinMode(ENCODER_BUTTON, INPUT_PULLDOWN_16);
  
  // Настройка прерываний
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), handleEncoder, CHANGE);
  
  // Можем добавить и на второй пин для большей точности
  // attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), handleEncoder, CHANGE);
  
  Serial.println("Энкодер готов. Крутите или нажмите кнопку.");
  Serial.println("LED на плате будет мигать при вращении.");
}

void loop() {
  // 1. Отображение позиции энкодера
  if (encoderPos != lastReportedPos) {
    Serial.print("Позиция: ");
    Serial.print(encoderPos);
    
    if (encoderPos > lastReportedPos) {
      Serial.println(" (Вправо)");
    } else {
      Serial.println(" (Влево)");
    }
    
    lastReportedPos = encoderPos;
    
    // Дополнительные действия при вращении
    // Например, изменение яркости, громкости и т.д.
  }
  
  // 2. Обработка кнопки с защитой от дребезга
  bool reading = digitalRead(ENCODER_BUTTON);
  
  // Для INPUT_PULLDOWN_16 логика инвертирована:
  // HIGH = нажата, LOW = отпущена
  if (reading == HIGH) { // Кнопка нажата
    if (millis() - lastDebounceTime > debounceDelay) {
      if (lastButtonState == LOW) { // Если было отпущено
        Serial.println("Кнопка нажата!");
        lastDebounceTime = millis();
        lastButtonState = HIGH;
        
        // Действия при нажатии кнопки
        // Например: encoderPos = 0; // Сброс счетчика
      }
    }
  } else { // Кнопка отпущена
    if (millis() - lastDebounceTime > debounceDelay) {
      if (lastButtonState == HIGH) { // Если было нажато
        Serial.println("Кнопка отпущена");
        lastDebounceTime = millis();
        lastButtonState = LOW;
      }
    }
  }
  
  // 3. Небольшая задержка для стабильности
  delay(10);
}