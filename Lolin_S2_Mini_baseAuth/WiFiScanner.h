#ifndef WiFiScanner_H
#define WiFiScanner_H

#include <WiFi.h>

class WiFiScanner {
public:
  void scanNetworks();
  int getNetworkCount();
  String getSSID(int index);
  int getRSSI(int index);

private:
  int networkCount = 0;
};

#endif
