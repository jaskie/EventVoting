// LoRaVoting.h

/*
Module supporting radio communication
*/

#ifndef _LORA_VOTING_h
#define _LORA_VOTING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif
#include "LoRaVotingMessage.h"

class LoRaVotingClass
{
private:
	uint8_t* _messageBuffer;
	size_t _messageBufferPos;
	void handleReceive(int packetSize);
	static void onReceive(int packetSize);
	bool isPacketReady();
	bool isValidMessageType(byte messageType);
	void discardBuffer();
	void parseMessage();
	void(*_receivedBroadcastCallback)(BroadcastMessage* message);

 public:
	 LoRaVotingClass();
	 ~LoRaVotingClass();
	 void init();
	 void SendMessage(BroadcastMessage& message);
	 void ReceivedBroadcastCallback(void(*callback)(BroadcastMessage* message));

};

extern LoRaVotingClass LoRaVoting;

#endif

