#ifndef FS_UTILS_H
#define FS_UTILS_H

#include <Arduino.h>
#include <LittleFS.h>

// Вспомогательная функция для чтения файла и замены плейсхолдеров
inline String getFileContent(const String& path, String (*processor)(const String&)) {
    File file = LittleFS.open(path, "r");
    if (!file) return String();
    String content = file.readString();
    file.close();
    if (processor) {
        // Можно было бы обработать плейсхолдеры, но мы делаем это в основном коде
    }
    return content;
}

// Функция для отправки файла клиенту
inline void sendFile(WebServer& server, const String& path, const String& contentType) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    server.send(200, contentType, file.readString());
    file.close();
}

#endif