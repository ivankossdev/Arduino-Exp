#ifndef _PAGE_H_
#include <Arduino.h>

class Page{
public: 
    Page();
    String mainPage; 
private:
    String index();

};

extern Page page;

#endif