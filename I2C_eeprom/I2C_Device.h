#ifndef _I2C_DEVICE_H_
#include <Wire.h> 
#include <Arduino.h>

#define LEN_MESSAGE 256

class I2C_Device{
  public:
    char *searchDevice();

  private:
    char message[LEN_MESSAGE] = {'\0'};
}; 

#endif