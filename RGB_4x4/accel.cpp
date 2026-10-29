#include "accel.h"
#include "config.h"
#include <Wire.h>
#include <math.h>

#define ADXL345_ADDR       0x53
#define REG_POWER_CTL      0x2D
#define REG_DATA_FORMAT    0x31
#define REG_BW_RATE        0x2C
#define REG_DATAX0         0x32

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

void Accelerometer::begin() {
  Wire.begin();
  writeReg(REG_DATA_FORMAT, 0x00);
  writeReg(REG_BW_RATE,     0x0A);
  writeReg(REG_POWER_CTL,   0x08);

  delay(10);
  calibrate();
}

IInput::Event Accelerometer::poll() {
  float ax, ay, az;
  readRaw(ax, ay, az);
  return decideEvent(ax, ay);
}

void Accelerometer::suppressUntilNeutral() {
  m_needNeutral = true;
}

void Accelerometer::recalibrate() {
  calibrate();
}

void Accelerometer::calibrate() {
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

  int16_t rawX = (int16_t)(buf[0] | (buf[1] << 8));
  int16_t rawY = (int16_t)(buf[2] | (buf[3] << 8));
  int16_t rawZ = (int16_t)(buf[4] | (buf[5] << 8));

  const float scale = 0.0039f;
  ax = rawX * scale;
  ay = rawY * scale;
  az = rawZ * scale;
}

IInput::Event Accelerometer::decideEvent(float ax, float ay) {
  float dx = ax - m_offsetX;
  float dy = ay - m_offsetY;

  if (m_needNeutral) {
    bool inNeutral = (fabs(dx) < NEUTRAL_ZONE_G && fabs(dy) < NEUTRAL_ZONE_G);
    if (inNeutral) {
      m_needNeutral = false;
    } else {
      return EV_NONE;
    }
  }

  float ax_mag = fabs(dx);
  float ay_mag = fabs(dy);
  if (ax_mag < TILT_THRESHOLD_G && ay_mag < TILT_THRESHOLD_G) {
    return EV_NONE;
  }

  bool  useX = (ax_mag > ay_mag);
  float mag  = useX ? ax_mag : ay_mag;

  float t = (mag - TILT_THRESHOLD_G) / (TILT_MAX_G - TILT_THRESHOLD_G);
  if (t < 0.0f) t = 0.0f;
  if (t > 1.0f) t = 1.0f;

  uint16_t cooldownMs =
      STEP_COOLDOWN_MAX_MS -
      (uint16_t)(t * (STEP_COOLDOWN_MAX_MS - STEP_COOLDOWN_MIN_MS));

  unsigned long now = millis();
  if (now - m_lastStepTime < cooldownMs) {
    return EV_NONE;
  }

  Event ev;
  if (useX) {
    ev = (dx > 0) ? EV_RIGHT : EV_LEFT;
  } else {
    ev = (dy > 0) ? EV_UP : EV_DOWN;
  }

  m_lastStepTime = now;
  return ev;
}