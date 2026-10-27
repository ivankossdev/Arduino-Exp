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
  Wire.requestFrom(ADXL345_ADDR, len, true);
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
  // Пока плата не вернётся в нейтраль — шагов нет.
  if (m_needNeutral) {
    bool inNeutral = (fabs(dx) < NEUTRAL_ZONE_G && fabs(dy) < NEUTRAL_ZONE_G);
    if (inNeutral) {
      m_needNeutral = false;
    } else {
      return EV_NONE;
    }
  }

  // --- Наклона нет → точка стоит ---
  if (fabs(dx) < TILT_THRESHOLD_G && fabs(dy) < TILT_THRESHOLD_G) {
    return EV_NONE;
  }

  // --- Кулдаун между шагами (скорость движения точки) ---
  unsigned long now = millis();
  if (now - m_lastStepTime < STEP_COOLDOWN_MS) {
    return EV_NONE;
  }

  // --- Доминирующая ось → направление ---
  Event ev = EV_NONE;
  if (fabs(dx) > fabs(dy)) {
    // Наклон по X: вправо → RIGHT, влево → LEFT
    ev = (dx > 0) ? EV_RIGHT : EV_LEFT;
  } else {
    // Наклон по Y: вперёд → UP, назад → DOWN
    // Если физически наоборот — поменяй EV_UP и EV_DOWN местами.
    ev = (dy > 0) ? EV_UP : EV_DOWN;
  }

  // Запоминаем время шага. m_needNeutral НЕ ставим —
  // поэтому, пока наклон держится, точка шагает непрерывно.
  m_lastStepTime = now;
  return ev;
}