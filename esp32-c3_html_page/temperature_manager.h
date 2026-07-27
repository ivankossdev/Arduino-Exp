#ifndef TEMPERATURE_MANAGER_H
#define TEMPERATURE_MANAGER_H

class TemperatureManager {
public:
  TemperatureManager(int startValue, int maxValue, int resetValue)
    : _value(startValue), _maxTemp(maxValue), _reset(resetValue) {}

  int get() const { return _value; }

  void update() {
    if (_value >= _maxTemp) {
      _value = _reset;
    } else {
      _value++;
    }
  }

private:
  int _value;
  int _maxTemp;
  int _reset;
};

#endif
