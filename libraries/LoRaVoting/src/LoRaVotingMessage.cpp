// 
// 
// 

#include "LoRaVotingMessage.h"

MessageType LoRaVotingMessage::GetType() const
{
	return _type;
}

BroadcastMessage::BroadcastMessage(const MessageType messageType, const String& content)
{
	_type = messageType;
	_content = content;
}

String BroadcastMessage::GetContent() const
{
	return _content;
}

ResponseMessage::ResponseMessage(const MessageType type, const byte * senderId, const byte * content, const byte contentLength):
	_messageId(1)
{
	_type = type;
	_content = new byte[contentLength];
	_contentLength = contentLength;
	//_messageId = 1;// ++_lastMessageid
	memcpy(_content, content, contentLength);
	memcpy(_senderId, senderId, DEVICE_ID_LENGTH);
}


ResponseMessage::~ResponseMessage()
{
	delete[] _content;
}

uint16_t ResponseMessage::GetMessageId() const
{
	return _messageId;
}



byte ResponseMessage::GetContentLength() const
{
	return _contentLength;
}

byte* ResponseMessage::GetContent() const
{
	return _content;
}

byte* ResponseMessage::GetSenderId() const
{
	return (byte*)_senderId;
}

ConfirmationMessage::ConfirmationMessage(const MessageType messageType, const byte receiverId[DEVICE_ID_LENGTH], const uint16_t messageId)
{
	_messageId = messageId;
	_type = messageType;
	memcpy(_receiverId, receiverId, DEVICE_ID_LENGTH);
}

bool ConfirmationMessage::IsConfirmationTo(const MessageType type, const uint16_t messageId, const byte receiverId[DEVICE_ID_LENGTH]) const
{
	return type == _type
		&& messageId == _messageId
		&& ReceiverIdEqual(_receiverId);
}

byte* ConfirmationMessage::GetReceiverId() const
{
	return (byte*)_receiverId;
}

uint16_t ConfirmationMessage::GetMessageId() const
{
	return _messageId;
}

bool ConfirmationMessage::ReceiverIdEqual(const byte receiverId[DEVICE_ID_LENGTH]) const
{
	return memcmp(_receiverId, receiverId, DEVICE_ID_LENGTH) == 0;
}
