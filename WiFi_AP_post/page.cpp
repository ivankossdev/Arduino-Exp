#include "page.h"

Page::Page(){
  style_ = "<style>\
        * {\
            margin: 0;\
            padding: 0;\
            box-sizing: border-box;\
        }\
        body {\
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\
            background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);\
            min-height: 100vh;\
            display: flex;\
            justify-content: center;\
            align-items: center;\
            padding: 20px;\
        }\
        .form-container {\
            background: rgba(255, 255, 255, 0.97);\
            backdrop-filter: blur(10px);\
            padding: 40px 35px;\
            border-radius: 20px;\
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.4), 0 5px 15px rgba(0, 0, 0, 0.3);\
            width: 100%;\
            max-width: 450px;\
            transition: transform 0.3s ease, box-shadow 0.3s ease;\
        }\
        .form-container:hover {\
            transform: translateY(-5px);\
            box-shadow: 0 25px 50px rgba(0, 0, 0, 0.5);\
        }\
        h2 {\
            text-align: center;\
            margin-bottom: 20px;\
            color: #1e3c72;\
            font-size: 2rem;\
            font-weight: 600;\
            letter-spacing: 1px;\
            display: flex;\
            align-items: center;\
            justify-content: center;\
            gap: 10px;\
        }\
        .greeting-text {\
            text-align: center;\
            color: #2a5298;\
            margin-bottom: 25px;\
            font-size: 1.1rem;\
            font-weight: 300;\
            border-bottom: 1px solid #d0d0d0;\
            padding-bottom: 15px;\
        }\
        .action-form {\
            display: flex;\
            flex-direction: column;\
            gap: 20px;\
        }\
        .input-group {\
            position: relative;\
        }\
        .input-group .field-icon {\
            position: absolute;\
            left: 15px;\
            top: 50%;\
            transform: translateY(-50%);\
            font-size: 1.3rem;\
            color: #1e3c72;\
            width: 24px;\
            text-align: center;\
            pointer-events: none;\
        }\
        .input-field {\
            width: 100%;\
            padding: 15px 20px 15px 50px;\
            border: 2px solid #e0e0e0;\
            border-radius: 50px;\
            font-size: 1rem;\
            transition: all 0.3s;\
            outline: none;\
            background: #f5f7fa;\
            color: #333;\
        }\
        .input-field:focus {\
            border-color: #1e3c72;\
            box-shadow: 0 0 0 4px rgba(30, 60, 114, 0.2);\
            background: #ffffff;\
        }\
        .input-field::placeholder {\
            color: #999;\
            font-style: italic;\
        }\
        .submit-btn {\
            background: linear-gradient(45deg, #1e3c72, #2a5298);\
            color: white;\
            border: none;\
            padding: 15px 30px;\
            border-radius: 50px;\
            font-size: 1.1rem;\
            font-weight: 600;\
            cursor: pointer;\
            transition: all 0.3s;\
            box-shadow: 0 4px 15px rgba(30, 60, 114, 0.4);\
            text-transform: uppercase;\
            letter-spacing: 1px;\
            display: flex;\
            align-items: center;\
            justify-content: center;\
            gap: 10px;\
        }\
        .submit-btn:hover {\
            background: linear-gradient(45deg, #163258, #1e3c72);\
            box-shadow: 0 6px 20px rgba(30, 60, 114, 0.6);\
            transform: scale(1.02);\
        }\
        .submit-btn:active {\
            transform: scale(0.98);\
        }\
        .hint {\
            text-align: center;\
            color: #555;\
            font-size: 0.9rem;\
            margin-top: 15px;\
        }\
        @media (max-width: 480px) {\
            .form-container {\
                padding: 30px 20px;\
            }\
            h2 {\
                font-size: 1.8rem;\
            }\
        }\
            </style>"; 
}

String Page::main(){
main_ = "<!DOCTYPE html>\n\
<html lang=\"ru\">\n\
<head>\n\
    <meta charset=\"UTF-8\">\n\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
    <title>Send String</title>" + style_ + \
    "</head>\n\
<body>\n\
    <div class=\"form-container\">\n\
        <h2>⚙️ Action</h2>\n\
        <div class=\"greeting-text\">Введите строку для отправки</div>\n\
        <form class=\"action-form\" id=\"sendForm\" action=\"/postform/\" method=\"post\">\n\
            <div class=\"input-group\">\n\
                <span class=\"field-icon\">📝</span>\n\
                <input type=\"text\" name=\"user_name\" id=\"inputData\" class=\"input-field\"\n\
                       placeholder=\"Введите сообщение\" required>\n\
            </div>\n\
            <button type=\"submit\" class=\"submit-btn\">\n\
                <span>📨</span> Send\n\
            </button>\n\
            <div class=\"hint\">\n\
                Нажмите «Send», чтобы отправить данные\n\
            </div>\n\
        </form>\n\
    </div>\n\
</body>\n\
</html>";
  return main_;
}

String Page::style(){
  return style_; 
}


Page page;