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

#define LORA_BUFFER_SIZE 0x100


class LoRaVotingClass
{
private:
	bool _isReady;
	uint8_t _messageBuffer[LORA_BUFFER_SIZE];
	volatile size_t _messageBufferPos;
	void handleReceive(int packetSize);
	static void onReceive(int packetSize);
	bool isPacketReady();
	bool isValidMessageType(byte messageType);
	bool isValidCRC();
	void discardBuffer();
	void parseMessage();
	void(*_receivedBroadcastCallback)(const BroadcastMessage& message);
	void(*_receivedResponseCallback)(const ResponseMessage& message);
	void(*_receivedConfirmationCallback)(const ConfirmationMessage& message);

 public:
	 void init();
	 bool IsReady();
	 void SendMessage(const BroadcastMessage& message);
	 void SendMessage(const ResponseMessage& message);
	 void SendMessage(const ConfirmationMessage& message);
	 void ReceivedBroadcastCallback(void(*callback)(const BroadcastMessage& message));
	 void ReceivedResponseCallback(void(*callback)(const ResponseMessage& message));
	 void ReceivedConfirmationCallback(void(*callback)(const ConfirmationMessage& message));
};

extern LoRaVotingClass LoRaVoting;

#endif

