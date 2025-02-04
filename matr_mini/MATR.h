#ifndef __MATR_H
#define __MATR_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class MATR
{
	public:
		MATR(uint8_t _intensity=4, byte dataPin=D7, byte clockPin=D5);
    uint8_t intensity;
		// void display();
		// void clear();

    // volatile uint8_t disBuffer[8]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    void sendCommand(byte led);
    void sendData(byte add, byte data);
    void send(byte data);

	protected:
    byte dataPin;
    byte clockPin;
};

#endif