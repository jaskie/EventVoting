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

ResponseMessage::ResponseMessage(const MessageType type, const byte * senderId, const byte * content, const byte contentLength)
{
	_type = type;
	_content = new byte[contentLength];
	_contentLength = contentLength;
	_messageId = 1;// ++_lastMessageid
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

bool ResponseMessage::SenderIdEqual(const byte senderId[DEVICE_ID_LENGTH]) const
{
	return memcmp(_senderId, senderId, DEVICE_ID_LENGTH) == 0;
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

bool ConfirmationMessage::IsConfirmationTo(ResponseMessage message)
{
	return message.GetType() == _type
		&& message.GetMessageId() == _messageId
		&& message.SenderIdEqual(_receiverId);
}

