#ifndef _DSP_DRIVER
#define _DSP_DRIVER

class DspDriver {
private:
  const int NEXT = 84;
public:
  unsigned char ex[504];
  void clearEx();
  void insertFig(unsigned char *arr, int lenArr, int posX, int posY);
  void shiftLeft(int row, int endCount);
};

#endif