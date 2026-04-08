#ifndef NTP_TIME_H
#define NTP_TIME_H

#include <NTPClient.h>
#include <WiFiUdp.h>
#include "config.h"

class NTPTime {
private:
    WiFiUDP ntpUDP;
    NTPClient timeClient;

public:
    NTPTime();
    void begin();
    void update();
    int getHour();
    int getMinute();
    int getSecond();
    unsigned long getEpochTime();
};

#endif
