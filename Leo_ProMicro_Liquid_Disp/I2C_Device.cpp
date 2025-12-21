#include "I2C_Device.h"

char *I2C_Device::searchDevice() {
  Wire.begin();
  for (int i = 0; i < 127; i++) {
    Wire.beginTransmission(i);
    
    int result = Wire.endTransmission();

    if (result == 0) {
      sprintf(message, "Found 0x%x", i);
    }
  }
  Wire.end();
  return message; 
}
