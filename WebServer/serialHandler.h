#ifndef _SERIALHANDLER_H
#define _SERIALHANDLER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "conv.h"
#include "connectData.h"
#include "MemHandler.h"
#define CMDCOUNT 8

class SerialHandler{
  public:
    void serialReader();
    bool isRead;
    String serialData; 
    void serialPrint();
}; 

class SerialMenu : private SerialHandler, private DataConvertor{
  public:
    void menu();
    String command[CMDCOUNT] = {"ip a", "set ip" , "wifi", "set wifi", "set action", "relay", "clear", "close"}; 
    bool isSetIP = false;

  private: 
    void ipAddress();
    void nameWiFi();
    void setWiFi();
    void relayControl();
    void help();
    void setIP();
    void setIpAction();
    void writeNetworkData(String message, int *data);
    bool confirmation(String question);
};

#endif