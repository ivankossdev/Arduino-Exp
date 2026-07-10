#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

// Инициализация АЦП
void initADC();

// Чтение сигнала, фильтрация, возвращает текущий уровень (дБ) с учётом шумового порога
float getCurrentLevel();

// Обновление пика (должно вызываться в каждом цикле)
void updatePeak(float currentDB);

// Получить текущее значение пика (дБ)
float getPeak();

// Принудительный сброс пика
void resetPeak();

#endif