// 
// Module supporting radio communication
// 

#include "LoRaVoting.h"
#include <LoRa.h>

#define PIN_CS 10         // LoRa radio chip select
#define PIN_RESET 9       // LoRa radio reset
#define PIN_IRQ 2         // change for your board; must be a hardware interrupt pin
#define SOH 0x01
#define CONFIRMATION 0xC0
#define RESPONSE 0x80
#define FREQUENCY 434100000


void LoRaVotingClass::init()
{
	LoRa.setPins(PIN_CS, PIN_RESET, PIN_IRQ);// set CS, reset, IRQ pin
	_isReady = LoRa.begin(FREQUENCY);             // initialize ratio 
	LoRa.onReceive(LoRaVotingClass::onReceive);
	LoRa.receive();
}

void LoRaVotingClass::SendMessage(const BroadcastMessage& message)
{
	String content = message.GetContent();
	size_t contentSize = content.length() + 1;
	byte contentBytes[contentSize];
	content.getBytes(contentBytes, contentSize);
	byte packet[contentSize + 2];
	packet[0] = message.GetType();
	packet[1] = (byte)contentSize;
	memcpy(packet + 2, contentBytes, contentSize);
	uint16_t crc = 0;
	for (byte i = 0; i < contentSize + 2; i++)
		crc += packet[i];
	LoRa.beginPacket();
	LoRa.write(SOH);
	LoRa.write(packet, contentSize + 2);
	LoRa.write(lowByte(crc));
	LoRa.write(highByte(crc));
	LoRa.endPacket();
	LoRa.receive();
	Serial.print("sended BroadcastMessage crc: ");
	Serial.println(crc);
}

void LoRaVotingClass::SendMessage(const ResponseMessage& message)
{
	Serial.println("preparing packet");

	size_t packetSize = message.GetContentLength() + DEVICE_ID_LENGTH + 4;
	byte packet[packetSize];
	size_t packetPos = 0;
	packet[packetPos++] = message.GetType() | RESPONSE;
	memcpy(packet + packetPos, message.GetSenderId(), DEVICE_ID_LENGTH);
	packetPos += DEVICE_ID_LENGTH;
	packet[packetPos++] = lowByte(message.GetMessageId());
	packet[packetPos++] = highByte(message.GetMessageId());
	packet[packetPos++] = message.GetContentLength();
	memcpy(packet + packetPos, message.GetContent(), message.GetContentLength());
	uint16_t crc = 0;
	for (packetPos = 0; packetPos < packetSize; packetPos++)
		crc += packet[packetPos];
	LoRa.beginPacket();
	LoRa.write(SOH);
	LoRa.write(packet, packetSize);
	LoRa.write(lowByte(crc));
	LoRa.write(highByte(crc));
	LoRa.endPacket();
	LoRa.receive();
	Serial.println("packet sent");
	Serial.print("sended ResponseMessage crc: ");
	Serial.println(crc);

}

void LoRaVotingClass::ReceivedBroadcastCallback(void(*callback)(const BroadcastMessage& message))
{
	_receivedBroadcastCallback = callback;
}

void LoRaVotingClass::ReceivedResponseCallback(void(*callback)(const ResponseMessage& message))
{
	_receivedResponseCallback = callback;
}


bool LoRaVotingClass::IsReady()
{
	return _isReady;
}

void LoRaVotingClass::handleReceive(int packetSize)
{
	if (packetSize == 0)
		return;
	Serial.print("packet received, bufferPos: ");
	Serial.println(_messageBufferPos);
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
				Serial.print("packet ready, MessageType: ");
				Serial.println(_messageBuffer[1]);
				parseMessage();
				discardBuffer();
			}
		}
	} while (LoRa.available());
}

void LoRaVotingClass::onReceive(int packetSize)
{
	LoRaVoting.handleReceive(packetSize);
}

bool LoRaVotingClass::isPacketReady()
{
	if (_messageBufferPos < 6U)
		return false;
	if ((_messageBuffer[1] & CONFIRMATION) == CONFIRMATION) // confirmation message
	{
		return _messageBufferPos >= 23U;
	}
	else if ((_messageBuffer[1] & RESPONSE) == RESPONSE)
	{
		return _messageBufferPos >= DEVICE_ID_LENGTH + 7U + _messageBuffer[20];
	}
	else // broadcast message
	{
		return _messageBufferPos >= 5U + _messageBuffer[2];
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

bool LoRaVotingClass::isValidCRC()
{
	if ((_messageBuffer[1] & CONFIRMATION) == CONFIRMATION)
	{
		uint16_t crc = 0;
		for (size_t i = 1; i < 20u; i++)
			crc += _messageBuffer[i];
		return lowByte(crc) == _messageBuffer[20]
			&& highByte(crc) == _messageBuffer[21];
	}
	else if ((_messageBuffer[1] & RESPONSE) == RESPONSE)
	{
		uint16_t crc = 0;
		for (size_t i = 1; i < DEVICE_ID_LENGTH + 5U + _messageBuffer[20] ; i++) // excluding CRC itself
			crc += _messageBuffer[i];
		return lowByte(crc) == _messageBuffer[_messageBuffer[20] + DEVICE_ID_LENGTH + 5U]
			&& highByte(crc) == _messageBuffer[_messageBuffer[20] + DEVICE_ID_LENGTH + 6U];
	}
	else // Broadcast
	{
		uint16_t crc = 0;
		for (size_t i = 1; i < _messageBuffer[2] + 3u; i++)
			crc += _messageBuffer[i];
		return lowByte(crc) == _messageBuffer[_messageBuffer[2] + 3U] 
			&& highByte(crc) == _messageBuffer[_messageBuffer[2] + 4U];
	}
}

void LoRaVotingClass::discardBuffer()
{
	_messageBufferPos = 0;
}

void LoRaVotingClass::parseMessage()
{
	Serial.print("buffer pos: ");
	Serial.println(_messageBufferPos);

	if (_messageBufferPos < 6)
		return;
	if (((_messageBuffer[1] & CONFIRMATION) == CONFIRMATION)
		&& (_messageBufferPos >= 23)) // confirmation message
	{
		byte messageType = _messageBuffer[1] & ~CONFIRMATION;
		if (!(isValidMessageType(messageType)) || !isValidCRC())
			return;

	}
	else if ((_messageBuffer[1] & RESPONSE) == RESPONSE)
	{
		if (!(isValidMessageType(_messageBuffer[1] & ~RESPONSE)) || !isValidCRC())
			return;
		if (_receivedResponseCallback)
		{
			ResponseMessage message((MessageType)(_messageBuffer[1] & ~RESPONSE), _messageBuffer + 2, _messageBuffer + 21, DEVICE_ID_LENGTH);
			_receivedResponseCallback(message);
		}
	}
	else
	{
		if (!(isValidMessageType(_messageBuffer[1])) || !isValidCRC())
			return;
		if (_receivedBroadcastCallback)
		{
			String s((char*)(_messageBuffer + 3));
			BroadcastMessage message((MessageType)_messageBuffer[1], s);
			_receivedBroadcastCallback(message);
		}
		return;
	}
}

LoRaVotingClass LoRaVoting;

