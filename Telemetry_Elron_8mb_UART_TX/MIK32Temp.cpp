#include "MIK32Temp.h"

extern "C" {
#include "mik32_hal_tsens.h"
#include "analog_reg.h"
}

// TSENS — единственный в чипе, поэтому держим один статический дескриптор.
static TSENS_HandleTypeDef s_htsens;
static bool                s_hwReady = false;

bool MIK32Temp::begin() {
    if (_initialized) return true;

    if (!s_hwReady) {
        s_htsens.Instance  = ANALOG_REG;
        s_htsens.Clock     = HAL_TSENS_SYS_CLK;
        s_htsens.Frequency = TSENS_OPTIMAL_FREQUENCY;

        if (HAL_TSENS_Init(&s_htsens) != HAL_OK) {
            return false;
        }

        // Явно снимаем сброс и даём схеме стабилизироваться.
        __HAL_TSENS_RELEASE_RESET(&s_htsens);
        delay(50);

        s_hwReady = true;
    }

    _initialized = true;
    return true;
}

float MIK32Temp::read() {
    if (!_initialized) return NAN;

    __HAL_TSENS_SINGLE_START(&s_htsens);

    uint32_t start = millis();
    while (!__HAL_TSENS_GET_EOC(&s_htsens)) {
        if (millis() - start > _timeoutMs) {
            return NAN;
        }
    }

    uint32_t raw   = __HAL_TSENS_READ_MEASUREMENT(&s_htsens);
    int32_t  centi = TSENS_VALUE_TO_CELSIUS(raw);
    return centi / 100.0f + _offset;
}

float MIK32Temp::readAveraged(uint8_t samples) {
    if (!_initialized || samples == 0) return NAN;

    float sum = 0.0f;
    uint8_t ok = 0;

    for (uint8_t i = 0; i < samples; i++) {
        float t = read();
        if (!isnan(t)) {
            sum += t;
            ok++;
        }
    }

    return ok > 0 ? (sum / ok) : NAN;
}

void MIK32Temp::startRead() {
    if (!_initialized) return;
    __HAL_TSENS_SINGLE_START(&s_htsens);
}

bool MIK32Temp::isReady() {
    if (!_initialized) return false;
    return __HAL_TSENS_GET_EOC(&s_htsens);
}

float MIK32Temp::getResult() {
    if (!_initialized) return NAN;
    uint32_t raw = __HAL_TSENS_READ_MEASUREMENT(&s_htsens);
    int32_t  centi = TSENS_VALUE_TO_CELSIUS(raw);
    return centi / 100.0f + _offset;
}