#include "mv.h"

void setup() {
  // Инициализация шины I2C (для ESP32-C3 можно указать пины, например 8 и 9)
  Wire.begin();  // или Wire.begin(I2C_SDA, I2C_SCL), если определено в config.h
  Serial.begin(115200);
  // Инициализация дисплея
  if (!drawShape.begin()) {
    // Если не удалось, можно зависнуть или вывести ошибку
    Serial.println(F("Display init failed, halt"));
    for (;;);
  }
}

void loop() {
  Logic_1();
  drawFrame();
}