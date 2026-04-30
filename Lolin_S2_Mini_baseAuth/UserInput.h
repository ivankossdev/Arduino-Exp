// UserInput.h
#ifndef UserInput_H
#define UserInput_H

#include <Arduino.h>
#include "WiFiScanner.h"

class UserInput {
public:
  String readSSID(WiFiScanner& scanner);  
  String readPassword();
  String readServerURL();
  bool confirmWithTimeout(unsigned long timeoutMs);
  bool confirmSelection();

private:
  String readLine();
};

#endif
