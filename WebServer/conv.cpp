#include "conv.h"

bool DataConvertor::checkIpAddress(String ipAddr) {
  int i = 0;
  int checkValue = 0;
  int dotCount = 0;
  int dotCount_ = 0;
  int segmentCount = 0;
  bool status = false;

  do {
    checkValue = ipAddr.c_str()[i] - 0x30;
    if ((checkValue >= 0 && checkValue <= 9) || checkValue == -2) {

      // Подсчет точек в строке их должно быть 3
      if (checkValue == -2) {
        dotCount++;
        if (dotCount > 3) {
          status = false;
          break;
        }
      }

      if (dotCount == 3) { dotCount_++; }

      // Подсчет символов в подстроке 1 - 3 не больше 
      if (checkValue != -2) {
        segmentCount++;
        if (segmentCount > 3) {
          status = false;
          dotCount = 0;
          break;
        }
      } else {
        segmentCount = 0;
      }

    } else {
      status = false;
      break;
    }
    i++;
  } while (ipAddr.c_str()[i] != '\0');

  if (dotCount == 3 && dotCount_ > 1) { status = true; }

  return status;
}

void DataConvertor::stringToIPaddress(String ipAddr, int *ip) {

  unsigned int len = ipAddr.length();
  unsigned int i = 0;

  int oldRes = 0;
  int res = 0;
  int arrayItem = 0;
  int hop = 0;

  while (i < len) {

    do {
      if (ipAddr.c_str()[i] != '.') {
        res = ipAddr.c_str()[i] - 0x30;

        if (hop == 0) {
          oldRes = res;
        } else if (hop > 0) {
          res = oldRes * 10 + res;
          oldRes = res;
        }
        hop++;
      }
      i++;
    } while (ipAddr.c_str()[i] != '.' && i < len);

    hop = 0;
    ip[arrayItem] = res;
    arrayItem++;
  }
}