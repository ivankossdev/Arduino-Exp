#include "SerialConsole.h"
#include "Settings.h"
#include "Breath.h"
#include "BlinkLed.h"
#include "Heartbeat.h"

#include <string.h>   // strlen, strcmp

// ============================================================
//  Приватное состояние модуля (видно только внутри этого файла)
// ============================================================
namespace {

const size_t kBufferSize = 32;
char   gBuffer[kBufferSize];
size_t gIndex = 0;

// --- Разбор числа из строки ---
// Берём все подряд идущие цифры, остальное игнорируем.
// Возвращаем 0, если цифр нет.
unsigned long parseNumber(const char* s) {
  unsigned long v = 0;
  while (*s >= '0' && *s <= '9') {
    v = v * 10 + (unsigned long)(*s - '0');
    s++;
  }
  return v;
}

// --- Справка ---
void printHelp() {
  Serial.println(F("Commands:"));
  Serial.println(F("  s<N>    breath speed 0..100"));
  Serial.println(F("  i<N>    blink interval, ms"));
  Serial.println(F("  h<N>    heartbeat interval, ms"));
  Serial.println(F("  stop    stop all timers"));
  Serial.println(F("  start   start all timers"));
  Serial.println(F("  ?       print current settings"));
}

// --- Статус ---
void printStatus() {
  Serial.print(F("[st] breath speed="));
  Serial.print(breathGetSpeed());
  Serial.print(breathIsRunning() ? F(" run") : F(" stop"));

  Serial.print(F(", blink="));
  Serial.print(blinkGetInterval());
  Serial.print(blinkIsRunning() ? F(" run") : F(" stop"));

  Serial.print(F(", hb="));
  Serial.print(heartbeatGetInterval());
  Serial.println(heartbeatIsRunning() ? F(" run") : F(" stop"));
}

// --- Обработка одной строки ---
void handleCommand(char* line) {
  // Убираем хвостовые пробелы и '\r', если строка пришла CRLF.
  size_t n = strlen(line);
  while (n > 0 && (line[n-1] == '\r' || line[n-1] == ' ' || line[n-1] == '\t')) {
    line[--n] = '\0';
  }
  if (n == 0) return;   // пустая строка — молча игнорируем

  if (strcmp(line, "?")    == 0) { printStatus(); return; }
  if (strcmp(line, "help") == 0) { printHelp();   return; }

  if (strcmp(line, "stop") == 0) {
    breathStop(); blinkStop(); heartbeatStop();
    Serial.println(F("all stopped"));
    return;
  }
  if (strcmp(line, "start") == 0) {
    breathStart(); blinkStart(); heartbeatStart();
    Serial.println(F("all started"));
    return;
  }

  switch (line[0]) {
    case 's': {
      unsigned long v = parseNumber(line + 1);
      if (v > 100) v = 100;
      breathSetSpeed((uint8_t)v);
      Serial.print(F("breath speed="));
      Serial.println(breathGetSpeed());
      break;
    }
    case 'i': {
      unsigned long v = parseNumber(line + 1);
      blinkSetInterval(v);
      Serial.print(F("blink interval="));
      Serial.println(blinkGetInterval());
      break;
    }
    case 'h': {
      unsigned long v = parseNumber(line + 1);
      heartbeatSetInterval(v);
      Serial.print(F("hb interval="));
      Serial.println(heartbeatGetInterval());
      break;
    }
    default:
      Serial.print(F("unknown: "));
      Serial.println(line);
      Serial.println(F("type 'help'"));
      break;
  }
}

}  // namespace

// ============================================================
//  Публичный интерфейс
// ============================================================

void consoleBegin() {
  gIndex = 0;
  Serial.println(F("Console ready. Type 'help'."));
  printStatus();
}

void consoleUpdate() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == '\n') {
      gBuffer[gIndex] = '\0';
      handleCommand(gBuffer);
      gIndex = 0;
    } else if (c != '\r') {
      if (gIndex < kBufferSize - 1) {
        gBuffer[gIndex++] = c;
      }
      // при переполнении просто отбрасываем лишние символы —
      // следующая строка стартует чисто
    }
  }
}