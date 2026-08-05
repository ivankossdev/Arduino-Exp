#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "WiFiManager.h"
#include "WiFiCredentials.h"   

class Menu {
private:
  static const int MAX_NETWORKS = 50;
  int _menuChoice;
  bool _scanning;
  bool _hasScanResult;
  WiFiCredentials _creds; 
  WiFiManager _scanner;
  String _lastSSID; 
  String _lastPassword; 
  NetworkInfo _networks[MAX_NETWORKS];

  void printMenu();
  void handleMenuChoice(int choice);
  void SerialBufferClear();
  void wifiScan();
  void displayNetworks(const NetworkInfo* networks, int count);
  void connectToNetwork();
  void saveCurrentNetwork();   
  void showSavedNetworks();  
  void connectToSavedNetwork();
  void deleteSavedNetwork();

  String getStatus() const;

  // НОВОЕ: автоматическое подключение при старте
  void autoConnect();

public:
  Menu();
  void begin();
  void update();
};

#endif