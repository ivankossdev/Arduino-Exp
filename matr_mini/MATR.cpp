#include "MATR.h"

MATR::MATR(uint8_t _intensity, byte dataPin, byte clockPin)
{
	this->dataPin = dataPin;
	this->clockPin = clockPin;

	if(_intensity>7) intensity=7;
	else intensity=_intensity;

	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	
	digitalWrite(dataPin, HIGH);
	digitalWrite(clockPin, HIGH);
}