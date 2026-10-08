/*
  Плавное мигание светодиода (PWM) на Elbear Ace-UNO без блокировки loop()
  Подключение: анод светодиода через резистор 220-330 Ом к D9, катод к GND.
*/

const int ledPin = 9;         // ШИМ-вывод D9
int brightness = 0;           // Текущая яркость (0-255)
int fadeAmount = 5;           // Шаг изменения яркости

unsigned long previousMillis = 0;   // Момент последнего обновления яркости
const unsigned long interval = 30;  // Интервал между шагами (мс)

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  // Проверяем, прошёл ли нужный интервал
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Сохраняем момент обновления

    analogWrite(ledPin, brightness); // Устанавливаем яркость

    brightness += fadeAmount;        // Изменяем яркость

    // Меняем направление на границах
    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount;
    }
  }

  // Здесь можно выполнять любую другую логику, не связанную с миганием
}