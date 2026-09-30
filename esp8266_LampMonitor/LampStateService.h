#ifndef LAMP_STATE_SERVICE_H
#define LAMP_STATE_SERVICE_H

#include <Arduino.h>
#include <functional>
#include "MqttService.h"

typedef std::function<void(const String& payload)> LampStateCallback;

class LampStateService {
public:
    explicit LampStateService(MqttService& mqttService);

    void setOnStateChange(LampStateCallback cb) { _onChange = cb; }

    bool handleMqttMessage(const String& topic, const String& payload);

    const String& getPayload() const { return _payload; }
    bool hasData() const { return _hasData; }

    void onMqttConnected();

private:
    MqttService& _mqttService;
    String _payload;
    bool _hasData = false;
    LampStateCallback _onChange;
};

#endif