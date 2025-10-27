#ifndef _SERIALHANDLER_H
#define _SERIALHANDLER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

class SerialHandler{
  public:
    void serialReader();
    String serialData; 
}; 

class SerialMenu : private SerialHandler{
  public:
    void menu();
};

#endif