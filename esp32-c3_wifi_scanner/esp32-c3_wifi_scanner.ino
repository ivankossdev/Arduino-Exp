#include "wifi_scanner.h"
#include <Ticker.h>

WiFiScanner scanner;
Ticker update;
const float updatePeriod = 30;

// Флаг, который говорит о необходимости начать сканирование
volatile bool scanRequested = false;

// Функция-обработчик таймера. Она только устанавливает флаг!
void requestScan() {
  scanRequested = true;
}

void setup() {
  Serial.begin(115200);
  // Таймер теперь вызывает функцию requestScan, а не scanner.scan
  update.attach(updatePeriod, requestScan);
}

void loop() {
  // В основном цикле проверяем флаг и запускаем сканирование
  if (scanRequested) {
    scanRequested = false; // Сбрасываем флаг
    scanner.scan();        // Теперь это безопасно!
  }
  
  // Здесь можно добавить небольшой delay для снижения нагрузки на процессор,
  // если в loop больше ничего нет.
  // delay(10);
}