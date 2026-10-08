#include "Console.h"
#include "Settings.h"

using namespace console_impl;

// ==================== PUBLIC ====================

Console::Console() : pos(0), onSettingsChanged(nullptr) {
  buf[0] = '\0';
}

void Console::setOnSettingsChanged(SettingsChangedCallback cb) {
  onSettingsChanged = cb;
}

void Console::printPrompt() {
  Serial.print("> ");
}

void Console::tick() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\r' || c == '\n') {
      if (pos > 0) {
        buf[pos] = '\0';
        Serial.println();
        handleLine(buf);
        pos = 0;
        printPrompt();
      }
      continue;
    }

    if (c == '\b' || c == 0x7F) {
      if (pos > 0) { pos--; Serial.print("\b \b"); }
      continue;
    }

    if (pos < MAX_LINE - 1) {
      buf[pos++] = c;
      Serial.print(c);
    }
  }
}

// ==================== ДИСПЕТЧЕР ====================

void Console::handleLine(char* line) {
  char* cursor = line;
  char* cmd = parseToken(&cursor);
  if (cmd == nullptr) return;

  if      (strEquals(cmd, "help"))  cmdHelp();
  else if (strEquals(cmd, "show"))  cmdShow();
  else if (strEquals(cmd, "set"))   cmdSet(cursor);
  else if (strEquals(cmd, "mode"))  cmdMode(cursor);
  else if (strEquals(cmd, "save"))  cmdSave();
  else if (strEquals(cmd, "reset")) cmdReset();
  else if (strEquals(cmd, "dump"))  cmdDump();
  else if (strEquals(cmd, "peek"))  cmdPeek(cursor);
  else if (strEquals(cmd, "poke"))  cmdPoke(cursor);
  else if (strEquals(cmd, "stats")) cmdStats();
  else if (strEquals(cmd, "led2"))  cmdLed2(cursor);
  else {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
    Serial.println("Type 'help' for list.");
  }
}

// ==================== БАЗОВЫЕ КОМАНДЫ ====================

void Console::cmdHelp() {
  Serial.println("Commands:");
  Serial.println("  help                 - this message");
  Serial.println("  show                 - display current settings");
  Serial.println("  mode <slow|fast|off> - switch operating mode");
  Serial.println("  set slow <ms>        - slow interval (50..60000)");
  Serial.println("  set fast <ms>        - fast interval (50..60000)");
  Serial.println("  set count <n>        - ticks per series (1..255)");
  Serial.println("  set longpress <ms>   - long press threshold (200..10000)");
  Serial.println("  save                 - write settings to EEPROM");
  Serial.println("  reset                - factory reset (ALL settings to defaults)");
  Serial.println("  dump                 - hex dump of EEPROM (first 32 bytes)");
  Serial.println("  peek <addr>          - read byte at address (0..1023)");
  Serial.println("  poke <addr> <val>    - write byte at address (val 0..255)");
  Serial.println("  stats                - show boot/button counters and uptime");
  Serial.println("  led2 <ms>            - LED2 blink interval (0=off, 50..60000)");
}

void Console::cmdShow() {
  Serial.println("--- Current settings ---");
  Serial.print("mode         = ");
  Serial.println(settings.mode == SLOW ? "SLOW" :
                 settings.mode == FAST ? "FAST" : "OFF");
  Serial.print("intervalSlow = "); Serial.println(settings.intervalSlow);
  Serial.print("intervalFast = "); Serial.println(settings.intervalFast);
  Serial.print("blinkCount   = "); Serial.println(settings.blinkCount);
  Serial.print("longPressMs  = "); Serial.println(settings.longPressMs);
  Serial.print("version      = "); Serial.println(settings.version);
}

void Console::cmdSet(char* args) {
  char* cursor = args;
  char* key = parseToken(&cursor);
  char* val = parseToken(&cursor);

  if (key == nullptr || val == nullptr) {
    Serial.println("Usage: set <param> <value>");
    Serial.println("  params: slow, fast, count, longpress");
    return;
  }

  long v = atol(val);
  bool ok = true;

  if (strEquals(key, "slow")) {
    if (v < 50 || v > 60000) { Serial.println("Range: 50..60000"); ok = false; }
    else { settings.intervalSlow = v; Serial.print("intervalSlow = "); Serial.println(v); }
  }
  else if (strEquals(key, "fast")) {
    if (v < 50 || v > 60000) { Serial.println("Range: 50..60000"); ok = false; }
    else { settings.intervalFast = v; Serial.print("intervalFast = "); Serial.println(v); }
  }
  else if (strEquals(key, "count")) {
    if (v < 1 || v > 255) { Serial.println("Range: 1..255"); ok = false; }
    else { settings.blinkCount = v; Serial.print("blinkCount = "); Serial.println(v); }
  }
  else if (strEquals(key, "longpress")) {
    if (v < 200 || v > 10000) { Serial.println("Range: 200..10000"); ok = false; }
    else { settings.longPressMs = v; Serial.print("longPressMs = "); Serial.println(v); }
  }
  else {
    Serial.print("Unknown param: "); Serial.println(key);
    ok = false;
  }

  if (ok) {
    settingsSave();
    if (onSettingsChanged) onSettingsChanged();
    Serial.println("Saved to EEPROM.");
  }
}

void Console::cmdMode(char* args) {
  char* cursor = args;
  char* arg = parseToken(&cursor);

  if (arg == nullptr) { Serial.println("Usage: mode <slow|fast|off>"); return; }

  if      (strEquals(arg, "slow")) settings.mode = SLOW;
  else if (strEquals(arg, "fast")) settings.mode = FAST;
  else if (strEquals(arg, "off"))  settings.mode = OFF;
  else { Serial.println("Unknown mode. Use: slow | fast | off"); return; }

  settingsSave();
  if (onSettingsChanged) onSettingsChanged();

  Serial.print("Mode -> ");
  Serial.println(settings.mode == SLOW ? "SLOW" :
                 settings.mode == FAST ? "FAST" : "OFF");
}

void Console::cmdSave() {
  settingsSave();
  Serial.println("Saved to EEPROM.");
}

void Console::cmdReset() {
  settingsFactoryReset();
  if (onSettingsChanged) onSettingsChanged();
}