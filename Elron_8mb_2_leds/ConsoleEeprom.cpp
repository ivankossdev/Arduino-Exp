#include "Console.h"
#include "Settings.h"
#include <EEPROM.h>

using namespace console_impl;

void Console::cmdDump() {
  const int DUMP_LEN = 32;
  uint8_t data[DUMP_LEN];
  for (int i = 0; i < DUMP_LEN; i++) data[i] = EEPROM.read(i);

  Serial.println("--- EEPROM hex dump (first 32 bytes) ---");
  const char* HEX_ = "0123456789ABCDEF";

  for (int base = 0; base < DUMP_LEN; base += 16) {
    Serial.print(HEX_[(base >> 12) & 0xF]);
    Serial.print(HEX_[(base >>  8) & 0xF]);
    Serial.print(HEX_[(base >>  4) & 0xF]);
    Serial.print(HEX_[ base        & 0xF]);
    Serial.print(": ");

    for (int i = 0; i < 16; i++) {
      uint8_t b = data[base + i];
      Serial.print(HEX_[(b >> 4) & 0xF]);
      Serial.print(HEX_[ b       & 0xF]);
      Serial.print(' ');
      if (i == 7) Serial.print(' ');
    }

    Serial.print(" |");
    for (int i = 0; i < 16; i++) {
      uint8_t b = data[base + i];
      Serial.print((b >= 32 && b < 127) ? (char)b : '.');
    }
    Serial.println('|');
  }

  Serial.println();
  Serial.println("Field map (packed Settings):");
  Serial.print("  byte  0       : mode         = "); Serial.println(settings.mode);
  Serial.print("  bytes 1..4    : intervalSlow = "); Serial.println(settings.intervalSlow);
  Serial.print("  bytes 5..8    : intervalFast = "); Serial.println(settings.intervalFast);
  Serial.print("  bytes 9..10   : blinkCount   = "); Serial.println(settings.blinkCount);
  Serial.print("  bytes 11..12  : longPressMs  = "); Serial.println(settings.longPressMs);
  Serial.print("  byte  13      : version      = "); Serial.println(settings.version);
  Serial.print("  sizeof(Settings) = ");           Serial.println(sizeof(Settings));
}

void Console::cmdPeek(char* args) {
  char* cursor = args;
  char* addrStr = parseToken(&cursor);

  if (addrStr == nullptr) { Serial.println("Usage: peek <addr>"); return; }

  long addr = atol(addrStr);
  if (addr < 0 || addr > 1023) {
    Serial.println("Address out of range (0..1023)");
    return;
  }

  uint8_t val = EEPROM.read(addr);
  Serial.print("EEPROM["); Serial.print(addr); Serial.print("] = 0x");
  if (val < 16) Serial.print('0');
  Serial.print(val, HEX);
  Serial.print(" ("); Serial.print(val); Serial.println(")");
}

void Console::cmdPoke(char* args) {
  char* cursor = args;
  char* addrStr = parseToken(&cursor);
  char* valStr  = parseToken(&cursor);

  if (addrStr == nullptr || valStr == nullptr) {
    Serial.println("Usage: poke <addr> <val>");
    return;
  }

  long addr = atol(addrStr);
  long val  = atol(valStr);

  if (addr < 0 || addr > 1023) {
    Serial.println("Address out of range (0..1023)"); return;
  }
  if (val < 0 || val > 255) {
    Serial.println("Value out of range (0..255)"); return;
  }

  uint8_t oldVal = EEPROM.read(addr);
  EEPROM.write(addr, (uint8_t)val);

  Serial.print("EEPROM["); Serial.print(addr); Serial.print("]: 0x");
  if (oldVal < 16) Serial.print('0');
  Serial.print(oldVal, HEX);
  Serial.print(" -> 0x");
  if (val < 16) Serial.print('0');
  Serial.print(val, HEX);
  Serial.println();

  settingsLoad();
  if (onSettingsChanged) onSettingsChanged();

  Serial.print("Mode -> ");
  Serial.println(settings.mode == SLOW ? "SLOW" :
                 settings.mode == FAST ? "FAST" : "OFF");
}