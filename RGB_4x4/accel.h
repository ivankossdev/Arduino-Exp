#pragma once
#include "input.h"

class Accelerometer : public IInput {
public:
  void  begin() override;
  Event poll() override;
  void  suppressUntilNeutral() override;

  /* Перекалибровать: текущее положение платы становится «нейтралью».
   * Блокирует ~100–120 мс (20 отсчётов × 5 мс + I2C).
   * Вызывать при входе в ACCEL, чтобы не тащить за собой
   * калибровку, сделанную при старте. */
  void  recalibrate();

private:
  static constexpr float    TILT_THRESHOLD_G     = 0.35f;
  static constexpr float    TILT_MAX_G           = 0.60f;
  static constexpr uint16_t STEP_COOLDOWN_MIN_MS = 100;
  static constexpr uint16_t STEP_COOLDOWN_MAX_MS = 300;
  static constexpr float    NEUTRAL_ZONE_G       = 0.15f;

  unsigned long m_lastStepTime = 0;
  bool          m_needNeutral  = false;
  float         m_offsetX = 0.0f;
  float         m_offsetY = 0.0f;

  void  readRaw(float &ax, float &ay, float &az);
  void  calibrate();
  Event decideEvent(float ax, float ay);
};