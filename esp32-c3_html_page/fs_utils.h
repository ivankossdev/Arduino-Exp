#ifndef FS_UTILS_H
#define FS_UTILS_H

#include <Arduino.h>
#include <LittleFS.h>
#include <WebServer.h>

String getFileContent(const String& path);
void sendFile(WebServer& server, const String& path, const String& contentType);

#endif
