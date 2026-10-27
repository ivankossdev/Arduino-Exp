#pragma once
#include <Arduino.h>

/* Модуль кнопок.
 * Отвечает только за: чтение пинов, антидребезг, latch,
 * приоритет при одновременном нажатии.
 * Про режимы, движение точки и вывод не знает ничего. */
class Buttons {
public:
  enum Event {
    EV_NONE = 0,
    EV_MODE,     // фронт нажатия MODE
    EV_UP,       // «свершился» один шаг вверх
    EV_DOWN,
    EV_LEFT,
    EV_RIGHT
  };

  /* Настроить пины и зафиксировать начальное состояние.
   * Вызывать в setup() до первого poll(). */
  void begin();

  /* Опрос. Вызывать с частотой ~100 Гц (каждые ~10 мс).
   * Возвращает одно событие за вызов или EV_NONE.
   * Дебаунс, latch и приоритеты — внутри. */
  Event poll();

  /* Игнорировать направляющие кнопки, пока все не отпустят.
   * Нужно при возврате в MANUAL: чтобы зажатая кнопка,
   * которую держали в AUTO, не дала «фантомный» шаг. */
  void lockUntilRelease();

private:
  // --- Состояние кнопки MODE ---
  bool          m_modeRaw    = false;
  bool          m_modeStable = false;
  unsigned long m_modeChange = 0;

  // --- Состояние направляющих кнопок ---
  int           m_dirRaw     = -1;
  unsigned long m_dirChange  = 0;
  bool          m_dirLocked  = false;
};