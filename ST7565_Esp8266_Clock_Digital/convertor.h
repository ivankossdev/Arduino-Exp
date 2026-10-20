#ifndef _CONVERTOR_H
#define _CONVERTOR_H

class Convertor {
public:
  int bcdToDec(int c);
  int decToBcd(int c);
  int tensDigit(int c);
  int onesDigit(int c);
};
#endif