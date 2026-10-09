#include "Breath.h"
#include "EventTimer.h"

/***********************************************************
  Тестовый  дерьмокод:
    - Проверка работы класса EventTimer 
    - Проверка вывода сообщения 
    - Шаг в будующее Сделать Класс LedManager
  Далее перепишем на архитектурное решение. 
 ***********************************************************/

// Экземпляр мигалки LED_BUILTIN
EventTimer ledBlink(1000); 

//  Колбэк функция для неблокирующей мигалки LED_BUILTIN
void onBlink(bool state) {
  digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
}

void setup() {

  // Инициализация серийного прота
  Serial.begin(9600);
  Serial.println();
  Serial.println("Start program!");

  // Инициализация порта мигалки
  pinMode(LED_BUILTIN, OUTPUT);

  // Регистрируем колбэк функцию
  ledBlink.setCallback(onBlink);

  // Инициализация PWM мигалки
  breathBegin();
}

void loop() {
  breathUpdate();
  ledBlink.tick(); 
}