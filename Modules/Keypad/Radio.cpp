// 
// Module supporting radio communication
// 

/*
	Packet format
	SOH

*/

#include "Radio.h"
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin
#define BUFFER_SIZE 0x100
#define SOH 0x01
#define CONFIRMATION 0xC0

RadioClass::RadioClass():
	_messageBufferPos(0)
{
	_messageBuffer = new uint8_t[BUFFER_SIZE];
}

RadioClass::~RadioClass()
{
	delete[] _messageBuffer;
}

void RadioClass::init()
{
	LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin
	LoRa.begin(433E6);             // initialize ratio 
	LoRa.receive();
	LoRa.onReceive(RadioClass::onReceive);
}

RadioMessage RadioClass::readMessage()
{
	return RadioMessage();
}


void RadioClass::handleReceive(int packetSize)
{
	if (packetSize == 0)
		return;  
	do 
	{
		if (_messageBufferPos >= BUFFER_SIZE - 1)
			discardBuffer();
		int received = LoRa.read();
		if (received == -1) // no data available
			continue;
		if (_messageBufferPos || received == SOH)
		{
			_messageBuffer[_messageBufferPos++] = (byte)received;
			if (isPacketReady())
			{
				parseMessage();
				discardBuffer();
			}
		}
	} 
	while (LoRa.available());
}

void RadioClass::onReceive(int packetSize)
{
	Radio.handleReceive(packetSize);
}

bool RadioClass::isPacketReady()
{
	if (_messageBufferPos < 6)
		return false;
	if ((_messageBuffer[1] && CONFIRMATION) == CONFIRMATION) // confirmation message
	{
		return _messageBufferPos >= 23;
	}
	else // broadcast message
	{
		return _messageBufferPos >= 6 + _messageBuffer[2];
	}
	return false;
}

void RadioClass::discardBuffer()
{
	_messageBufferPos = 0;
}

void RadioClass::parseMessage()
{
}

RadioClass Radio;

