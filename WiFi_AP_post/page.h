#ifndef _PAGE_H_
#include <Arduino.h>

class Page{
public: 
    String main();
private:
    String main_ = ""; 
};

extern Page page;

#endif