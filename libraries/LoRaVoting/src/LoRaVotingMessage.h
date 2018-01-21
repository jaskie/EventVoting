// LoRaVotingMessage.h

#ifndef _LORA_VOTING_MESSAGE_h
#define _LORA_VOTING_MESSAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define DEVICE_ID_LENGTH 16u

typedef enum MessageType: byte
{
	Empty,
	RegisteredDevices,
	NewDevice,
	DropDevice,
	StartVoting,
	EndVoting,
	Display
} MessageType;

class LoRaVotingMessage
{
protected:
	MessageType _type;
public:
	MessageType GetType() const;
};

// a message sent by master controller to device
class BroadcastMessage : public LoRaVotingMessage
{
private:
	String _content;
public:
	BroadcastMessage(const MessageType messageType, const String& content);
	String GetContent() const;
};

// response sent to master controller
class ResponseMessage : public LoRaVotingMessage
{
	byte* _content;
	byte _contentLength;
	byte _senderId[DEVICE_ID_LENGTH];
	uint16_t _messageId;
	//static uint16_t _lastMessageid;
public:
	ResponseMessage(const MessageType type, const byte* senderId, const byte* content, const byte contentLength);
	virtual ~ResponseMessage();
	uint16_t GetMessageId() const;
	byte GetContentLength() const;
	byte* GetContent() const; 
	byte* GetSenderId() const;
	bool SenderIdEqual(const byte senderId[DEVICE_ID_LENGTH]) const;
};

// a response confirmation sent by master controller
class ConfirmationMessage : public LoRaVotingMessage
{
private:
	byte _receiverId[DEVICE_ID_LENGTH];
	uint16_t _messageId;
public:
	ConfirmationMessage(const MessageType messageType, const byte receiverId[DEVICE_ID_LENGTH], const uint16_t messageId);
	bool IsConfirmationTo(ResponseMessage message);
};
#endif

