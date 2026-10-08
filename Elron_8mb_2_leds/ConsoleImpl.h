#ifndef CONSOLE_IMPL_H
#define CONSOLE_IMPL_H

#include <Arduino.h>

// Вспомогательные утилиты для парсинга команд Console.
// inline — чтобы каждый .cpp видел их без дублирования кода
// и без необходимости заводить отдельный .cpp.

namespace console_impl {

// Регистро-независимое сравнение C-строк.
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

// Разбор строки по пробелам.
// Возвращает следующий токен или nullptr, если токенов больше нет.
// *cursor сдвигается на позицию после разделителя.
inline char* parseToken(char** cursor) {
  char* p = *cursor;
  if (p == nullptr) return nullptr;

  while (*p == ' ') p++;                // пропускаем ведущие пробелы
  if (*p == '\0') { *cursor = p; return nullptr; }

  char* start = p;
  while (*p != ' ' && *p != '\0') p++;  // ищем конец токена
  if (*p == ' ') { *p = '\0'; p++; }    // затираем пробел нулём

  *cursor = p;
  return start;
}

}  // namespace console_impl

#endif // CONSOLE_IMPL_H