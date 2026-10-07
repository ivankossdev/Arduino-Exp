#ifndef CONSOLE_H
#define CONSOLE_H

#include <Arduino.h>

// Колбэк, который дергается после успешного изменения настроек.
// Его задача — применить новые settings к таймеру, кнопке и т.д.
typedef void (*SettingsChangedCallback)();

class Console {
public:
  Console();

  void setOnSettingsChanged(SettingsChangedCallback cb);

  // Вызывать из loop(). Не блокирует: читает только то, что уже пришло.
  void tick();

  // Напечатать приглашение "> "
  void printPrompt();

private:
  static const uint8_t MAX_LINE = 64;

  char     buf[MAX_LINE];
  uint8_t  pos;
  SettingsChangedCallback onSettingsChanged;

  void handleLine(char* line);
  void cmdHelp();
  void cmdShow();
  void cmdSet(char* args);
  void cmdMode(char* args);
  void cmdSave();
  void cmdReset();
  void cmdDump();
  void cmdPeek(char* args); 
  void cmdPoke(char* args);
  void cmdStats();
};

#endif // CONSOLE_H