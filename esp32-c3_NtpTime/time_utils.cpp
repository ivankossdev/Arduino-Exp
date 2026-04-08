#include "time_utils.h"

const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool TimeUtils::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

void TimeUtils::unixToDate(unsigned long epochTime, int& year, int& month, int& day) {
    long daysSinceEpoch = epochTime / 86400L;
    int y = 1970;
    int daysInYear;

    while (true) {
        daysInYear = isLeapYear(y) ? 366 : 365;
        if (daysSinceEpoch >= daysInYear) {
            daysSinceEpoch -= daysInYear;
            y++;
        } else {
            break;
        }
    }

    year = y;

    bool leap = isLeapYear(year);
    int m = 1;
    while (m <= 12) {
        int daysThisMonth = (m == 2 && leap) ? 29 : daysInMonth[m - 1];
        if (daysSinceEpoch < daysThisMonth) {
            day = daysSinceEpoch + 1;  // +1, потому что дни начинаются с 1
            month = m;
            return;
        }
        daysSinceEpoch -= daysThisMonth;
        m++;
    }
}

String TimeUtils::formatTwoDigits(int number) {
    if (number < 10) {
        return "0" + String(number);
    }
    return String(number);
}
