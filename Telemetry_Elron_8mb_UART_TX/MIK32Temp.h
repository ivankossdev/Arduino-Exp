#ifndef MIK32_TEMP_H
#define MIK32_TEMP_H

#include <Arduino.h>

/**
 * Драйвер встроенного датчика температуры кристалла MIK32 (TSENS).
 *
 * Особенности:
 *  - Измеряет температуру самого чипа, а не воздуха.
 *  - Разрешение ~0.5 °C, разрешение регистра 10 бит.
 *  - Одно измерение занимает ~100 мс.
 *  - На чипе только ОДИН блок TSENS, поэтому все объекты
 *    разделяют общий аппаратный ресурс. Создавайте один экземпляр.
 *
 * Пример:
 *    MIK32Temp temp;
 *    if (!temp.begin()) { ... }
 *    float t = temp.read();   // NAN при ошибке
 */
class MIK32Temp {
public:
  MIK32Temp() = default;

  /** Инициализация TSENS. Возвращает false при ошибке. */
  bool begin();

  /** Одиночное измерение. Возвращает °C или NAN при таймауте. */
  float read();

  /** Запустить измерение в фоне (не блокирует) */
  void startRead();

  /** Проверить, готов ли результат */
  bool isReady();

  /** Забрать результат (вызывать только если isReady() вернул true) */
  float getResult();

  /** Усреднённое измерение. Возвращает °C или NAN, если ни одно не удалось. */
  float readAveraged(uint8_t samples = 4);

  /** Готов ли датчик к работе (был ли успешный begin). */
  bool isReady() const {
    return _initialized;
  }

  /** Калибровочная поправка, добавляемая к каждому измерению, °C. */
  void setOffset(float offset) {
    _offset = offset;
  }
  float getOffset() const {
    return _offset;
  }

  /** Таймаут одного измерения, мс (по умолчанию 500). */
  void setTimeout(uint32_t ms) {
    _timeoutMs = ms;
  }
  uint32_t getTimeout() const {
    return _timeoutMs;
  }

private:
  bool _initialized = false;
  float _offset = 0.0f;
  uint32_t _timeoutMs = 500;
};

#endif  // MIK32_TEMP_H