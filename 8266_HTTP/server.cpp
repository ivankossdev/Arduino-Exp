#include "server.h"

void showInfo() {
  mylcd.LCDClear(0x00);
  mylcd.LCDgotoXY(0, 0);
  mylcd.printf("Status OK.");
  mylcd.LCDgotoXY(0, 1);
  mylcd.printf("IP: ");
  mylcd.LCDgotoXY(0, 2);
  mylcd.print(WiFi.localIP());
}

void actionOn() {
  server.send(200, "text/plain", "Evidence: \nAction ON");
  mylcd.LCDClear(0x00);
  mylcd.LCDgotoXY(0, 0);
  mylcd.printf("Evidence: ");
  mylcd.LCDgotoXY(0, 1);
  mylcd.printf("Action ON");
  delay(1000);
  showInfo();
}

void actionOff() {
  server.send(200, "text/plain", "Evidence: \nAction OFF");
  mylcd.LCDClear(0x00);
  mylcd.LCDgotoXY(0, 0);
  mylcd.printf("Evidence: ");
  mylcd.LCDgotoXY(0, 1);
  mylcd.printf("Action OFF");
  delay(1000);
  showInfo();
}

void motion() {
  server.send(200, "text/plain", "Evidence: \nCamera motion");
  mylcd.LCDClear(0x00);
  mylcd.LCDgotoXY(0, 0);
  mylcd.printf("Evidence: ");
  mylcd.LCDgotoXY(0, 1);
  mylcd.printf("Camera 1");
  mylcd.LCDgotoXY(0, 2);
  mylcd.printf("Motion");
  delay(2000);
  showInfo();
}