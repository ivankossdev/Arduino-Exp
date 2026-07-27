#include "fs_utils.h"

String getFileContent(const String& path) {
  File file = LittleFS.open(path, "r");
  if (!file) return String();
  String content = file.readString();
  file.close();
  return content;
}

void sendFile(WebServer& server, const String& path, const String& contentType) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    server.send(404, "text/plain", "File not found");
    return;
  }
  server.send(200, contentType, file.readString());
  file.close();
}
