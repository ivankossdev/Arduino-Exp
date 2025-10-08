#ifndef __CONNECTION_H
#define __CONNECTION_H

#include "Arduino.h"
#include <EEPROM.h>
#include "display.h"

class Connection{
  public:
    void enterSerialDataInArray(char *data, String message);

  private:
    String serialData;
};

static Connection connection;
#endif