// 
// 
// 

#include "RadioMessage.h"

MessageType RadioMessage::GetType() const
{
	return _type;
}

BroadcastMessage::BroadcastMessage(const MessageType messageType, const byte content[], const size_t& length)
{
	_type = messageType;
	_content = new byte[length];
	_length = length;
	memcpy(_content, content, length * sizeof(byte));
}

BroadcastMessage::~BroadcastMessage()
{
	delete[] _content;
}

size_t BroadcastMessage::GetContent(byte*& content) const
{
	content = new byte[_length];
	memcpy(content, _content, _length * sizeof(byte));
	return _length;
}

