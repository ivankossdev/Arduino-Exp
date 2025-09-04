#include <ESP8266WiFi.h>
#include "display.h"
#include <cstring>

MyDisplay dsp(&display);

void setup() {
  delay(100);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  dsp.displayInit();
}

void loop() {
  dsp.displayPrintText((char *)"WiFi scan");
  scan();
  dsp.foundClear();
  dsp.displayClear();
}

void scan() {
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  bool hidden;
  int scanResult;

  dsp.displayPrintText((char *)"Starting WiFi scan...");

  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);

  if (scanResult == 0) {
    Serial.println(F("No networks found"));
  } else if (scanResult > 0) {

    dsp.displayClear();
    sprintf(dsp.found, "%d networks found:", scanResult);
    dsp.displayPrintText(dsp.found);
    dsp.foundClear();

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
      sprintf(dsp.found, "%02d:CH %02d %s", i, channel, ssid.c_str());
      dsp.displayPrintText(dsp.found);
      
      yield();
    }
  } else {
    Serial.printf(PSTR("WiFi scan error %d"), scanResult);
  }

  // Wait a bit before scanning again
  delay(10000);
}
