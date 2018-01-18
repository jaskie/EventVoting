// Radio.h

/*
Module supporting radio communication
*/

#ifndef _RADIO_h
#define _RADIO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif
#include "RadioMessage.h"

class RadioClass
{
private:
	uint8_t* _messageBuffer;
	size_t _messageBufferPos;
	void handleReceive(int packetSize);
	static void onReceive(int packetSize);
	bool isPacketReady();
	void discardBuffer();
	void parseMessage();

 public:
	 RadioClass();
	 ~RadioClass();
	 void init();
	 RadioMessage readMessage();
};

extern RadioClass Radio;

#endif

