#ifndef _SERIALHANDLER_H
#define _SERIALHANDLER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#define CMDCOUNT 6

class SerialHandler{
  public:
    void serialReader();
    bool isRead;
    String serialData; 
    void serialPrint();
}; 

class SerialMenu : private SerialHandler{
  public:
    void menu();
    String command[CMDCOUNT] = {"ip a", "set ip" , "wifi", "relay","clear", "close"}; 
    bool isSetIP = false;

  private: 
    void ipAddress();
    void nameWiFi();
    void relayControl();
    void help();
    void controllerName();
    void setIP();
    bool confirmation(String question);
};

#endif