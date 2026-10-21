#include <Arduino.h>

extern "C" {
#include "mik32_hal_tsens.h"
#include "analog_reg.h"
}

TSENS_HandleTypeDef htsens;

bool tsensInit() {
  htsens.Instance  = ANALOG_REG;
  htsens.Clock     = HAL_TSENS_SYS_CLK;
  htsens.Frequency = TSENS_OPTIMAL_FREQUENCY;

  if (HAL_TSENS_Init(&htsens) != HAL_OK) {
    return false;
  }
  delay(50); // стабилизация после включения
  return true;
}

// Возвращает температуру в °C или NAN при ошибке
float tsensRead() {
  __HAL_TSENS_SINGLE_START(&htsens);

  uint32_t start = millis();
  while (!__HAL_TSENS_GET_EOC(&htsens)) {
    if (millis() - start > 500) {
      return NAN; // таймаут
    }
  }

  uint32_t raw = __HAL_TSENS_READ_MEASUREMENT(&htsens);
  int32_t  centi = TSENS_VALUE_TO_CELSIUS(raw);
  return centi / 100.0f;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!tsensInit()) {
    Serial.println("TSENS init failed");
    while (1);
  }
  Serial.println("TSENS ready");
}

void loop() {
  float t = tsensRead();
  if (isnan(t)) {
    Serial.println("TSENS read error");
  } else {
    Serial.print("Chip temp: ");
    Serial.print(t, 2);
    Serial.println(" °C");
  }
  delay(1000);
}