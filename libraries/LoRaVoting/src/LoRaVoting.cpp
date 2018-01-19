// 
// Module supporting radio communication
// 

#include "LoRaVoting.h"
#include <LoRa.h>

#define PIN_CS 10         // LoRa radio chip select
#define PIN_RESET 9       // LoRa radio reset
#define PIN_IRQ 2         // change for your board; must be a hardware interrupt pin
#define BUFFER_SIZE 0x100
#define SOH 0x01
#define CONFIRMATION 0xC0
#define FREQUENCY 433150000

LoRaVotingClass::LoRaVotingClass():
	_messageBufferPos(0)
{
	_messageBuffer = new uint8_t[BUFFER_SIZE];
}

LoRaVotingClass::~LoRaVotingClass()
{
	delete[] _messageBuffer;
}

void LoRaVotingClass::init()
{
	LoRa.setPins(PIN_CS, PIN_RESET, PIN_IRQ);// set CS, reset, IRQ pin
	LoRa.begin(FREQUENCY);             // initialize ratio 
	LoRa.receive();
	LoRa.onReceive(LoRaVotingClass::onReceive);
}

void LoRaVotingClass::SendMessage(BroadcastMessage & message)
{
	size_t packetSize = message.GetContentLength();
	byte * packet = new byte[packetSize + 2];
	packet[0] = message.GetType();
	packet[1] = message.GetContentLength();
	memcpy(packet + 2, message.GetContent(), packetSize * sizeof(byte));
	uint16_t crc = 0;
	for (size_t i = 0; i < packetSize; i++)
		crc += packet[i];
	LoRa.beginPacket();
	LoRa.write(SOH);
	LoRa.write(packet, packetSize);
	LoRa.write(lowByte(crc));
	LoRa.write(highByte(crc));
	LoRa.endPacket();
	delete[] packet;
}

void LoRaVotingClass::ReceivedBroadcastCallback(void(*callback)(BroadcastMessage *message))
{
	_receivedBroadcastCallback = callback;
}

void LoRaVotingClass::handleReceive(int packetSize)
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

void LoRaVotingClass::onReceive(int packetSize)
{
	LoRaVoting.handleReceive(packetSize);
}

bool LoRaVotingClass::isPacketReady()
{
	if (_messageBufferPos < 6U)
		return false;
	if ((_messageBuffer[1] && CONFIRMATION) == CONFIRMATION) // confirmation message
	{
		return _messageBufferPos >= 23U;
	}
	else // broadcast message
	{
		return _messageBufferPos >= 6U + _messageBuffer[2];
	}
	return false;
}

bool LoRaVotingClass::isValidMessageType(byte messageType)
{
	return messageType == RegisteredDevices 
		|| messageType == NewDevice 
		|| messageType == DropDevice 
		|| messageType == StartVoting 
		|| messageType == EndVoting 
		|| messageType == Display;
}

void LoRaVotingClass::discardBuffer()
{
	_messageBufferPos = 0;
}

void LoRaVotingClass::parseMessage()
{
	if (_messageBufferPos < 6)
		return;
	if (((_messageBuffer[1] && CONFIRMATION) == CONFIRMATION)
		&& (_messageBufferPos >= 23)) // confirmation message
	{
		byte messageType = _messageBuffer[1] && !CONFIRMATION;
		if (!(isValidMessageType(messageType)))
			return;
	}
	if (!(_messageBuffer[1] && CONFIRMATION)
		&& (_messageBufferPos >= 6 + _messageBuffer[2]))
	{
		if (!(isValidMessageType(_messageBuffer[1])))
			return;

	}
}

LoRaVotingClass LoRaVoting;

