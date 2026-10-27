#pragma once
#include "input.h"

/* Управление наклоном.
 * - Наклон удерживается → точка шагает каждые STEP_COOLDOWN_MS мс.
 * - Нейтраль → точка стоит.
 * - suppressUntilNeutral() → блокирует шаги, пока плата не вернётся в нейтраль
 *   (используется при переключении AUTO → MANUAL). */
class Accelerometer : public IInput {
public:
  void  begin() override;
  Event poll() override;
  void  suppressUntilNeutral() override;

private:
  // --- Настройки ---
  static constexpr float    TILT_THRESHOLD_G = 0.35f; // порог наклона (~20°)
  static constexpr uint16_t STEP_COOLDOWN_MS = 200;   // пауза между шагами
  static constexpr float    NEUTRAL_ZONE_G   = 0.15f; // зона «покоя»

  // --- Состояние ---
  unsigned long m_lastStepTime = 0;
  bool          m_needNeutral  = false;   // ставится только suppressUntilNeutral()
  float         m_offsetX = 0.0f;
  float         m_offsetY = 0.0f;

  // --- Внутренние методы ---
  void  readRaw(float &ax, float &ay, float &az);
  void  calibrate();
  Event decideEvent(float ax, float ay);
};