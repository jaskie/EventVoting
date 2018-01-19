// RadioMessage.h

#ifndef _RADIOMESSAGE_h
#define _RADIOMESSAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define DEVICE_ID_LENGTH 16

typedef enum MessageType: uint8_t
{
	Empty,
	RegisteredDevices,
	NewDevice,
	DropDevice,
	StartVoting,
	EndVoting,
	Display
} MessageType;

class RadioMessage
{
protected:
	MessageType _type;
public:
	MessageType GetType() const;
};

// a message sent by master controller to device
class BroadcastMessage : public RadioMessage
{
private:
	byte* _content;
	size_t _contentLength;
public:
	BroadcastMessage(const MessageType messageType, const byte content[], const size_t length);
	virtual ~BroadcastMessage();
	size_t GetContentLength() const;
	byte * GetContent();
};

// response sent to master controller
class ResponseMessage : public RadioMessage
{
	byte* _content;
	size_t _contentLength;
	byte _receiverId[DEVICE_ID_LENGTH];
	uint16_t _messageId;
	static uint16_t _lastMessageid;
public:
	ResponseMessage(const BroadcastMessage& question, const byte receiverId[DEVICE_ID_LENGTH], const byte content[], const size_t length);
	virtual ~ResponseMessage();
	uint16_t GetMessageId();
	bool ReceiverIdEqual(const byte receiverId[DEVICE_ID_LENGTH]);
};

// a response confirmation sent by master controller
class ConfirmationMessage : public RadioMessage
{
private:
	byte _receiverId[DEVICE_ID_LENGTH];
	uint16_t _messageId;
public:
	ConfirmationMessage(const MessageType messageType, const byte receiverId[DEVICE_ID_LENGTH], const uint16_t messageId);
	bool IsConfirmationTo(ResponseMessage message);
};
#endif

