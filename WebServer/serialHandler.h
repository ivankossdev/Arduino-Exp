#ifndef _SERIALHANDLER_H
#define _SERIALHANDLER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "conv.h"
#include "connectData.h"
#include "MemHandler.h"
#define CMDCOUNT 9

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
    String command[CMDCOUNT] = {"get ip", "set ip" , "get wifi", "set wifi", "set action", "get action", "relay", "clear", "close"}; 
    bool isSetIP = false;

  private: 
    void getIpAddress();
    void nameWiFi();
    void setWiFi();
    void relayControl();
    void help();
    void setIpAddress();
    void setIpAction();
    void getIpAction();
    void writeNetworkData(String message, int *data);
    bool confirmation(String question);
    void getNetworkInMemory();
};

#endif