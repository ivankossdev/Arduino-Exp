#ifndef CONSOLE_H
#define CONSOLE_H

#include <Arduino.h>

typedef void (*SettingsChangedCallback)();

// Хелперы парсинга. inline, чтобы каждый .cpp видел их без дублирования.
namespace console_impl {

// Регистро-независимое сравнение строк
inline bool strEquals(const char* a, const char* b) {
  while (*a && *b) {
    char ca = *a, cb = *b;
    if (ca >= 'A' && ca <= 'Z') ca += 32;
    if (cb >= 'A' && cb <= 'Z') cb += 32;
    if (ca != cb) return false;
    a++; b++;
  }
  return *a == '\0' && *b == '\0';
}

// Разбор строки по пробелам. *cursor сдвигается на следующий токен.
inline char* parseToken(char** cursor) {
  char* p = *cursor;
  if (p == nullptr) return nullptr;
  while (*p == ' ') p++;
  if (*p == '\0') { *cursor = p; return nullptr; }
  char* start = p;
  while (*p != ' ' && *p != '\0') p++;
  if (*p == ' ') { *p = '\0'; p++; }
  *cursor = p;
  return start;
}

}  // namespace console_impl

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
};

#endif // CONSOLE_H