#include "ntp_time.h"

NTPTime::NTPTime() : timeClient(ntpUDP, NTP_SERVER, TIME_OFFSET, UPDATE_INTERVAL) {}

void NTPTime::begin() {
    timeClient.begin();
}

void NTPTime::update() {
    timeClient.update();
}

int NTPTime::getHour() {
    return timeClient.getHours();
}

int NTPTime::getMinute() {
    return timeClient.getMinutes();
}

int NTPTime::getSecond() {
    return timeClient.getSeconds();
}

unsigned long NTPTime::getEpochTime() {
    return timeClient.getEpochTime();
}
