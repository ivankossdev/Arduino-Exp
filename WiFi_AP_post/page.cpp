#include "page.h"

String Page::main(){
main_ = "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
    <meta charset=\"UTF-8\">\n\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
    <title>Send String </title>\n\
    <style>\n\
        button,input,select,textarea,optgroup{font:inherit;margin:0}optgroup{font-weight:bold}\n\
        button,input{overflow:visible}button,select{text-transform:none}\n\
        button,[type=button],[type=reset],[type=submit]{-webkit-appearance:button}\n\
        button::-moz-focus-inner,[type=button]::-moz-focus-inner,[type=reset]::-moz-focus-inner,[type=submit]::-moz-focus-inner{border-style:none;padding:0}\n\
        button:-moz-focusring,[type=button]:-moz-focusring,[type=reset]:-moz-focusring,[type=submit]:-moz-focusring{outline:1px dotted ButtonText}\n\
        html{box-sizing:border-box}*,*:before,*:after{box-sizing:inherit}\n\
        html,body{font-family:Verdana,sans-serif;font-size:15px;line-height:1.5}\n\
        html,body{font-family:Verdana,sans-serif;font-size:15px;line-height:1.5;}html{overflow-x:hidden}\n\
        html{overflow-x:hidden;}\n\
        .w3-blue-grey,.w3-hover-blue-grey:hover,.w3-blue-gray,.w3-hover-blue-gray:hover{color:#fff!important;background-color:#607d8b!important}\n\
        .w3-container,.w3-panel{padding:0.01em 16px}.w3-panel{margin-top:16px;margin-bottom:16px}\n\
        .w3-container,.w3-panel{padding:0.01em 16px}.w3-panel{margin-top:16px;margin-bottom:16px}\n\
        .w3-btn,.w3-button{border:none;display:inline-block;padding:8px 16px;vertical-align:middle;overflow:hidden;text-decoration:none;color:inherit;background-color:inherit;text-align:center;cursor:pointer;white-space:nowrap}\n\
        .w3-btn:hover{box-shadow:0 8px 16px 0 rgba(0,0,0,0.2),0 6px 20px 0 rgba(0,0,0,0.19)}\n\
        .w3-black,.w3-hover-black:hover{color:#fff!important;background-color:#000!important}\n\
        .w3-display-topmiddle{position:absolute;left:50%;top:0;transform:translate(-50%,0%);-ms-transform:translate(-50%,0%)}\n\
    </style>\n\
</head>\n\
<body class=\"w3-container w3-blue-grey\">\n\
    <div class=\"w3-container\">\n\
        <div class=\"w3-display-topmiddle\">\n\
            <h2>Action</h2>\n\
            <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\n\
                <label for=\"name\">Name </label><br/>\n\
                <input type=\"text\" name=\"user_name\" /><br/>\n\
                <button type=\"submit\" class=\"w3-btn w3-black\">Action</button>\n\
            </form>\n\
        </div>\n\
    </div>\n\
</body>\n\
</html>\n";
  return main_;
}


Page page;