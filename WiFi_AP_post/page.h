#ifndef _PAGE_H_
#include <Arduino.h>

class Page{
public: 
    Page();
    String main();
    String style();
private:
    String main_ = ""; 
    String style_ = ""; 
};

extern Page page;

#endif