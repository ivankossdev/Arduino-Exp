#ifndef _CONV_H_
#define _CONV_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>

class DataConvertor{
  public:
    void stringToIPaddress(String ipAddr);
    int ip[4] = {0}; 
};

#endif