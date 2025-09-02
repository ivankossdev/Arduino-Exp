#include <ESP8266WiFi.h>
#include "display.h"
#include <cstring>

char found[512] = {'\0'};

void setup() {
  delay(10000);

  Serial.begin(115200);
  Serial.println(F("\nESP8266 WiFi scan example"));
  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);
  // Disconnect from an AP if it was previously connected
  WiFi.disconnect();
  delay(100);
  displayInit(&display);
}

void loop() {
  displayPrintText(&display, (char *)"WiFi scan");
  scan();
  foundClear();
  displayClear(&display);
}

void foundClear(){
  for(int i = 0; i < 512; i++) found[i] = '\0';
}

void scan() {
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  bool hidden;
  int scanResult;

  Serial.println(F("Starting WiFi scan..."));
  displayPrintText(&display, (char *)"Starting WiFi scan...");

  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);

  if (scanResult == 0) {
    Serial.println(F("No networks found"));
  } else if (scanResult > 0) {
    Serial.printf(PSTR("%d networks found:\n"), scanResult);

    displayClear(&display);
    sprintf(found, "%d networks found:", scanResult);
    displayPrintText(&display, found);
    foundClear();

    // Print unsorted scan results
    for (int8_t i = 0; i < scanResult; i++) {
      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);

      // get extra info
      const bss_info *bssInfo = WiFi.getScanInfoByIndex(i);
      String phyMode;
      const char *wps = "";
      if (bssInfo) {
        phyMode.reserve(12);
        phyMode = F("802.11");
        String slash;
        if (bssInfo->phy_11b) {
          phyMode += 'b';
          slash = '/';
        }
        if (bssInfo->phy_11g) {
          phyMode += slash + 'g';
          slash = '/';
        }
        if (bssInfo->phy_11n) {
          phyMode += slash + 'n';
        }
        if (bssInfo->wps) {
          wps = PSTR("WPS");
        }
      }
      sprintf(found, "%02d:[CH %02d] %s", i, channel, ssid.c_str());
      displayPrintText(&display, found);
      Serial.printf(PSTR("  %02d: [CH %02d] [%02X:%02X:%02X:%02X:%02X:%02X] %ddBm %c %c %-11s %3S %s\n"), i, channel, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], rssi, (encryptionType == ENC_TYPE_NONE) ? ' ' : '*', hidden ? 'H' : 'V', phyMode.c_str(), wps, ssid.c_str());
      yield();
    }
  } else {
    Serial.printf(PSTR("WiFi scan error %d"), scanResult);
  }
  Serial.println(F("!!!END!!!\n"));

  // Wait a bit before scanning again
  delay(10000);
}
