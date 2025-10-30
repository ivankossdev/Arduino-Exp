#ifndef _SERIALHANDLER_H
#define _SERIALHANDLER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#define CMDCOUNT 5

class SerialHandler{
  public:
    void serialReader();
    String serialData; 
    void serialPrint();
}; 

class SerialMenu : private SerialHandler{
  public:
    void menu();
    String command[CMDCOUNT] = {"ip a", "wifi", "relay","clear", "close"}; 

  private: 
    void ipAddress();
    void nameWiFi();
    void relayControl();
    void help();
};

#endif