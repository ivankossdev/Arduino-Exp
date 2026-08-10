#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "AppState.h"

class Menu {
private:
    int _menuChoice;
    AppState& _appState;

    void printMenu();
    void handleMenuChoice(int choice);
    void SerialBufferClear();
    void displayNetworksFromState();
    void connectToNetwork();
    void connectToSavedNetwork();
    void deleteSavedNetwork();
    void displayMqttSettings();
    void editMqttSettings();

    // Новый вспомогательный метод для ввода индекса
    int promptIndex(int max, const char* message);

public:
    Menu(AppState& appState);
    void begin();
    void update();
};

#endif