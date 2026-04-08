#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>

class TimeUtils {
public:
    static bool isLeapYear(int year);
    static void unixToDate(unsigned long epochTime, int& year, int& month, int& day);
    static String formatTwoDigits(int number);
};

#endif
