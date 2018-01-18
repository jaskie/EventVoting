// RadioMessage.h

#ifndef _RADIOMESSAGE_h
#define _RADIOMESSAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

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


class BroadcastMessage : public RadioMessage
{
private:
	byte* _content;
	size_t _length;
public:
	BroadcastMessage(const MessageType messageType, const byte content[], const size_t& length);
	~BroadcastMessage();
	size_t GetContent(byte*& content) const;
};
#endif

