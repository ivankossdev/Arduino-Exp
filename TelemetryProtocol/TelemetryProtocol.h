#ifndef TELEMETRY_PROTOCOL_H
#define TELEMETRY_PROTOCOL_H

#include <Arduino.h>   // включает Print, uint8_t и т.д.

// --------------------- Константы протокола ---------------------
#define PROTOCOL_STX          0xAA
#define PROTOCOL_ESC          0xAB
#define PROTOCOL_ESC_STX      0x01
#define PROTOCOL_ESC_ESC      0x02
#define PROTOCOL_MAX_DATA_LEN 64
#define PROTOCOL_MAX_PACKET   128

// Идентификаторы пакетов
enum PacketId : uint8_t {
    PID_PING    = 0x01,
    PID_DIGITAL = 0x02,
    PID_ANALOG  = 0x03,
    PID_STATUS  = 0x04,
    PID_EXTENDED= 0x05
};

// Тип обработчика пакетов
typedef void (*PacketHandler)(uint8_t id, uint8_t seq, const uint8_t* data, uint8_t len);

// --------------------- Класс протокола ---------------------
class TelemetryProtocol {
public:
    // Конструктор теперь принимает Print& (подходит для Serial, Serial1, SoftwareSerial и т.д.)
    TelemetryProtocol(Print& serial, bool useStuffing = true);

    // Отправка
    void sendPing();
    void sendDigital(uint16_t ports);
    void sendAnalog(const uint16_t* values, uint8_t count); // count <= 8
    void sendStatus(uint8_t flags, int16_t temp, uint16_t voltage);
    void sendCustom(uint8_t id, const uint8_t* data, uint8_t dataLen);

    // Приём
    void setHandler(uint8_t id, PacketHandler handler);
    void processByte(uint8_t byte);
    void resetReceiver();

private:
    Print& _serial;          // теперь ссылка на Print
    bool _useStuffing;
    uint8_t _seq;

    enum RxState : uint8_t {
        RX_WAIT_STX,
        RX_WAIT_LEN,
        RX_WAIT_DATA,
        RX_WAIT_CRC
    };
    RxState _rxState;
    uint8_t _rxLen;
    uint8_t _rxIndex;
    uint8_t _rxBuffer[PROTOCOL_MAX_DATA_LEN + 2];
    uint8_t _rxCrc;
    bool _escape;

    static const uint8_t MAX_HANDLERS = 16;
    PacketHandler _handlers[MAX_HANDLERS];

    static uint8_t calcCrc(const uint8_t* buf, uint8_t len);
    void sendPacket(uint8_t id, const uint8_t* data, uint8_t dataLen);
};

#endif