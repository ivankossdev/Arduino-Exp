#include "TelemetryProtocol.h"

TelemetryProtocol::TelemetryProtocol(Print& serial, bool useStuffing)
    : _serial(serial), _useStuffing(useStuffing), _seq(0), _escape(false) {
    resetReceiver();
    for (uint8_t i = 0; i < MAX_HANDLERS; ++i) {
        _handlers[i] = nullptr;
    }
}

void TelemetryProtocol::sendPing() {
    sendPacket(PID_PING, nullptr, 0);
}

void TelemetryProtocol::sendDigital(uint16_t ports) {
    uint8_t data[2];
    data[0] = ports & 0xFF;
    data[1] = (ports >> 8) & 0xFF;
    sendPacket(PID_DIGITAL, data, 2);
}

void TelemetryProtocol::sendAnalog(const uint16_t* values, uint8_t count) {
    if (count > 8) count = 8;
    uint8_t data[16];
    for (uint8_t i = 0; i < count; ++i) {
        data[2*i]   = values[i] & 0xFF;
        data[2*i+1] = (values[i] >> 8) & 0xFF;
    }
    sendPacket(PID_ANALOG, data, count * 2);
}

void TelemetryProtocol::sendStatus(uint8_t flags, int16_t temp, uint16_t voltage) {
    uint8_t data[5];
    data[0] = flags;
    data[1] = temp & 0xFF;
    data[2] = (temp >> 8) & 0xFF;
    data[3] = voltage & 0xFF;
    data[4] = (voltage >> 8) & 0xFF;
    sendPacket(PID_STATUS, data, 5);
}

void TelemetryProtocol::sendCustom(uint8_t id, const uint8_t* data, uint8_t dataLen) {
    if (dataLen > PROTOCOL_MAX_DATA_LEN) dataLen = PROTOCOL_MAX_DATA_LEN;
    sendPacket(id, data, dataLen);
}

void TelemetryProtocol::sendPacket(uint8_t id, const uint8_t* data, uint8_t dataLen) {
    uint8_t packet[PROTOCOL_MAX_PACKET];
    uint8_t len = 2 + dataLen;

    packet[0] = PROTOCOL_STX;
    packet[1] = len;
    packet[2] = id;
    packet[3] = _seq;
    if (dataLen && data) {
        memcpy(&packet[4], data, dataLen);
    }

    uint8_t crc = calcCrc(&packet[1], len);
    packet[1 + len] = crc;

    uint8_t totalBytes = len + 3;

    if (_useStuffing) {
        for (uint8_t i = 0; i < totalBytes; ++i) {
            uint8_t b = packet[i];
            if (b == PROTOCOL_STX || b == PROTOCOL_ESC) {
                _serial.write(PROTOCOL_ESC);
                _serial.write((b == PROTOCOL_STX) ? PROTOCOL_ESC_STX : PROTOCOL_ESC_ESC);
            } else {
                _serial.write(b);
            }
        }
    } else {
        _serial.write(packet, totalBytes);
    }

    _seq++;
}

void TelemetryProtocol::setHandler(uint8_t id, PacketHandler handler) {
    if (id < MAX_HANDLERS) {
        _handlers[id] = handler;
    }
}

void TelemetryProtocol::resetReceiver() {
    _rxState = RX_WAIT_STX;
    _escape = false;
    _rxIndex = 0;
}

void TelemetryProtocol::processByte(uint8_t byte) {
    if (_useStuffing) {
        if (_escape) {
            _escape = false;
            if (byte == PROTOCOL_ESC_STX)      byte = PROTOCOL_STX;
            else if (byte == PROTOCOL_ESC_ESC) byte = PROTOCOL_ESC;
            else {
                resetReceiver();
                return;
            }
        } else if (byte == PROTOCOL_ESC) {
            _escape = true;
            return;
        }
    }

    switch (_rxState) {
        case RX_WAIT_STX:
            if (byte == PROTOCOL_STX) {
                _rxState = RX_WAIT_LEN;
            }
            break;

        case RX_WAIT_LEN:
            if (byte < 2 || byte > (PROTOCOL_MAX_DATA_LEN + 2)) {
                resetReceiver();
                break;
            }
            _rxLen = byte;
            _rxIndex = 0;
            _rxState = RX_WAIT_DATA;
            break;

        case RX_WAIT_DATA:
            _rxBuffer[_rxIndex++] = byte;
            if (_rxIndex == _rxLen) {
                _rxState = RX_WAIT_CRC;
            }
            break;

        case RX_WAIT_CRC:
            _rxCrc = byte;
            _rxState = RX_WAIT_STX;
            if (calcCrc(_rxBuffer, _rxLen) == _rxCrc) {
                uint8_t id  = _rxBuffer[0];
                uint8_t seq = _rxBuffer[1];
                uint8_t dataLen = _rxLen - 2;
                const uint8_t* data = &_rxBuffer[2];

                if (id < MAX_HANDLERS && _handlers[id] != nullptr) {
                    _handlers[id](id, seq, data, dataLen);
                }
            }
            break;
    }
}

uint8_t TelemetryProtocol::calcCrc(const uint8_t* buf, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; ++i) {
        crc ^= buf[i];
    }
    return crc;
}