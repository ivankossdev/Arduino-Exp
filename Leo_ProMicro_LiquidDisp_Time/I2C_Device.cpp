#include "I2C_Device.h"

char *I2C_Device::searchDevice() {
  sprintf(message, "I2C: Found\n");
  char search[16] = {'\0'}; 

  Wire.begin();
  for (int i = 0; i < 127; i++) {
    Wire.beginTransmission(i);
    
    int result = Wire.endTransmission();

    if (result == 0) {
      sprintf(search, "0x%x \n", i);
      strcat(message, search);
    }
  }
  Wire.end();
  return message; 
}
