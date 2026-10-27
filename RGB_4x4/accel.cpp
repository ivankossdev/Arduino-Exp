#include "accel.h"
#include "config.h"
#include <Wire.h>
#include <math.h>

// ---------- Регистры ADXL345 ----------
#define ADXL345_ADDR       0x53
#define REG_POWER_CTL      0x2D
#define REG_DATA_FORMAT    0x31
#define REG_BW_RATE        0x2C
#define REG_DATAX0         0x32

// ---------- Низкоуровневый I²C ----------

static void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

static void readRegs(uint8_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)ADXL345_ADDR, len);
  for (uint8_t i = 0; i < len && Wire.available(); i++) {
    buf[i] = Wire.read();
  }
}

// ---------- Публичные методы ----------

void Accelerometer::begin() {
  Wire.begin();

  // ±2g, 10-bit, measurement mode, 100 Hz ODR
  writeReg(REG_DATA_FORMAT, 0x00);
  writeReg(REG_BW_RATE,     0x0A);
  writeReg(REG_POWER_CTL,   0x08);

  delay(10);       // дать датчику выйти в режим измерения
  calibrate();     // замерить смещение нуля в покое
}

IInput::Event Accelerometer::poll() {
  float ax, ay, az;
  readRaw(ax, ay, az);
  return decideEvent(ax, ay);
}

void Accelerometer::suppressUntilNeutral() {
  m_needNeutral = true;
}

// ---------- Внутренние методы ----------

void Accelerometer::calibrate() {
  // Усредняем 20 отсчётов в покое, чтобы найти смещение нуля
  float sx = 0.0f, sy = 0.0f;
  for (int i = 0; i < 20; i++) {
    float ax, ay, az;
    readRaw(ax, ay, az);
    sx += ax;
    sy += ay;
    delay(5);
  }
  m_offsetX = sx / 20.0f;
  m_offsetY = sy / 20.0f;
}

void Accelerometer::readRaw(float &ax, float &ay, float &az) {
  uint8_t buf[6];
  readRegs(REG_DATAX0, buf, 6);

  // Данные 16-битные, little-endian, масштаб ~3.9 mg/LSB при ±2g
  int16_t rawX = (int16_t)(buf[0] | (buf[1] << 8));
  int16_t rawY = (int16_t)(buf[2] | (buf[3] << 8));
  int16_t rawZ = (int16_t)(buf[4] | (buf[5] << 8));

  const float scale = 0.0039f;   // 3.9 mg = 0.0039 g
  ax = rawX * scale;
  ay = rawY * scale;
  az = rawZ * scale;
}

IInput::Event Accelerometer::decideEvent(float ax, float ay) {
  // Компенсация смещения нуля
  float dx = ax - m_offsetX;
  float dy = ay - m_offsetY;

  // --- Мягкий стоп после suppressUntilNeutral() ---
  if (m_needNeutral) {
    bool inNeutral = (fabs(dx) < NEUTRAL_ZONE_G && fabs(dy) < NEUTRAL_ZONE_G);
    if (inNeutral) {
      m_needNeutral = false;
    } else {
      return EV_NONE;
    }
  }

  // --- Наклона нет → точка стоит ---
  float ax_mag = fabs(dx);
  float ay_mag = fabs(dy);
  if (ax_mag < TILT_THRESHOLD_G && ay_mag < TILT_THRESHOLD_G) {
    return EV_NONE;
  }

  // --- Доминирующая ось (по ней же — и направление, и сила) ---
  bool  useX = (ax_mag > ay_mag);
  float mag  = useX ? ax_mag : ay_mag;   // 0..~1 (в единицах g)

  // --- Кулдаун зависит от силы наклона ---
  // t = 0 при пороге (TILT_THRESHOLD_G), t = 1 при TILT_MAX_G и выше.
  float t = (mag - TILT_THRESHOLD_G) / (TILT_MAX_G - TILT_THRESHOLD_G);
  if (t < 0.0f) t = 0.0f;
  if (t > 1.0f) t = 1.0f;

  uint16_t cooldownMs =
      STEP_COOLDOWN_MAX_MS -
      (uint16_t)(t * (STEP_COOLDOWN_MAX_MS - STEP_COOLDOWN_MIN_MS));

  // --- Проверка паузы ---
  unsigned long now = millis();
  if (now - m_lastStepTime < cooldownMs) {
    return EV_NONE;
  }

  // --- Направление по доминирующей оси ---
  Event ev;
  if (useX) {
    ev = (dx > 0) ? EV_RIGHT : EV_LEFT;
  } else {
    ev = (dy > 0) ? EV_UP : EV_DOWN;
  }

  m_lastStepTime = now;
  return ev;
}