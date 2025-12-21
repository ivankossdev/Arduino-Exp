#include "I2C_Device.h"

char *I2C_Device::searchDevice() {
  sprintf(message, "I2C: ");
  char search[16] = {'\0'}; 

  Wire.begin();
  for (int i = 0; i < 127; i++) {
    Wire.beginTransmission(i);
    
    int result = Wire.endTransmission();

    if (result == 0) {
      sprintf(search, "Found 0x%x ", i);
      strcat(message, search);
    }
  }
  Wire.end();
  return message; 
}
