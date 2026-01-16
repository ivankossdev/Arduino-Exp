#include "page.h"

Page::Page(){
  style_ = "<style>html{ font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; background-color: darkgray;} \
.button {background-color: #195B6A; border: none; color: white;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; width: 250px; height: 50px;} \
#labelData{font-size: 40px;}h2{font-size: 60px; margin-bottom: 5px;}#inputData{margin: 2px; width: 250px; height: 30px;}</style>"; 
}

String Page::main(){
main_ = "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
    <meta charset=\"UTF-8\">\n\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
    <title>Send String </title>\n" + style_ + \
"</head>\n\
<body>\n\
    <h2>Action</h2>\
    <form action=\"/postform/\" method=\"post\">\
        <label for=\"data\" id=\"labelData\">Message</label><br/>\
        <input type=\"text\" name=\"input data\" id=\"inputData\"/><br/>\
        <button type=\"submit\" class=\"button\">Send</button>\
    </form>\
</body>\n\
</html>\n";
  return main_;
}

String Page::style(){
  return style_; 
}


Page page;