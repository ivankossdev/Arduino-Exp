#ifndef CONSOLE_H
#define CONSOLE_H

#include <Arduino.h>
#include "ConsoleImpl.h"      // ← хелперы парсинга

typedef void (*SettingsChangedCallback)();

class Console {
public:
  Console();
  void setOnSettingsChanged(SettingsChangedCallback cb);
  void tick();
  void printPrompt();

private:
  static const uint8_t MAX_LINE = 64;

  char     buf[MAX_LINE];
  uint8_t  pos;
  SettingsChangedCallback onSettingsChanged;

  // Ядро (Console.cpp)
  void handleLine(char* line);
  void cmdHelp();
  void cmdShow();
  void cmdSet(char* args);
  void cmdMode(char* args);
  void cmdSave();
  void cmdReset();

  // EEPROM-отладка (ConsoleEeprom.cpp)
  void cmdDump();
  void cmdPeek(char* args);
  void cmdPoke(char* args);

  // Прикладное (ConsoleApp.cpp)
  void cmdStats();
  void cmdLed2(char* args);
  void cmdClearStats();
};

#endif // CONSOLE_H