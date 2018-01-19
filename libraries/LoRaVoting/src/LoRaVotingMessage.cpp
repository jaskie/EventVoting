// 
// 
// 

#include "LoRaVotingMessage.h"

MessageType LoRaVotingMessage::GetType() const
{
	return _type;
}

BroadcastMessage::BroadcastMessage(const MessageType messageType, const byte content[], const size_t length)
{
	_type = messageType;
	_content = new byte[length];
	_contentLength = length;
	memcpy(_content, content, length * sizeof(byte));
}

BroadcastMessage::~BroadcastMessage()
{
	delete[] _content;
}

size_t BroadcastMessage::GetContentLength() const
{
	return _contentLength;
}

byte * BroadcastMessage::GetContent() 
{
	return _content;
}

ResponseMessage::ResponseMessage(const BroadcastMessage& question, const byte receiverId[DEVICE_ID_LENGTH], const byte content[], const size_t length)
{
	_type = question.GetType();
	_content = new byte[length];
	_contentLength = length;
	_messageId = ++_lastMessageid;
	memcpy(_content, content, length * sizeof(byte));
}

ResponseMessage::~ResponseMessage()
{
	delete[] _content;
}

uint16_t ResponseMessage::GetMessageId()
{
	return _messageId;
}

bool ResponseMessage::ReceiverIdEqual(const byte receiverId[DEVICE_ID_LENGTH])
{
	return memcmp(_receiverId, receiverId, DEVICE_ID_LENGTH * sizeof(byte)) == 0;
}

ConfirmationMessage::ConfirmationMessage(const MessageType messageType, const byte receiverId[DEVICE_ID_LENGTH], const uint16_t messageId)
{
	_messageId = messageId;
	_type = messageType;
	memcpy(_receiverId, receiverId, DEVICE_ID_LENGTH * sizeof(byte));
}

bool ConfirmationMessage::IsConfirmationTo(ResponseMessage message)
{
	return message.GetType() == _type
		&& message.GetMessageId() == _messageId
		&& message.ReceiverIdEqual(_receiverId);
}

