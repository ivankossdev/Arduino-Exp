// UserInput.h
#ifndef UserInput_H
#define UserInput_H

#include <Arduino.h>

class UserInput {
public:
  String readSSID();
  String readPassword();
  bool confirmSelection();

private:
  String readLine();
};

#endif
