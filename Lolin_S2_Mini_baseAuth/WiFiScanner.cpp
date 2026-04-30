#include "WiFiScanner.h"

void WiFiScanner::scanNetworks() {
  Serial.println("Scanning for WiFi networks...");
  networkCount = WiFi.scanNetworks();

  if (networkCount == 0) {
    Serial.println("No networks found");
  } else {
    Serial.print("Found ");
    Serial.print(networkCount);
    Serial.println(" networks:");

    for (int i = 0; i < networkCount; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (RSSI: ");
      Serial.print(WiFi.RSSI(i));
      Serial.println(")");
    }
  }
}

int WiFiScanner::getNetworkCount() {
  return networkCount;
}

String WiFiScanner::getSSID(int index) {
  if (index >= 0 && index < networkCount) {
    return WiFi.SSID(index);
  }
  return "";
}

int WiFiScanner::getRSSI(int index) {
  if (index >= 0 && index < networkCount) {
    return WiFi.RSSI(index);
  }
  return 0;
}
