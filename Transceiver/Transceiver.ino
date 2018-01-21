#include <LoRaVoting.h>
#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
	Serial.begin(9600);                   // initialize serial
	LoRaVoting.init();
	LoRaVoting.ReceivedResponseCallback(onReceiveResponse);
	if (LoRaVoting.IsReady())
		Serial.println("LoRaVoting transceiver ready");
}

void loop() {
  // put your main code here, to run repeatedly:
	LoRaVoting.SendMessage(BroadcastMessage(RegisteredDevices, "asking for registered devices"));
	//LoRaVoting.SendMessage(BroadcastMessage(NewDevice, "ABCDEFGHIJKLMNOP"));
	delay(5000);
}


void onReceiveResponse(const ResponseMessage& message) 
{
	Serial.print("received response: ");
	Serial.print(message.GetType());
	Serial.print(" from ");
	Serial.print((char*)message.GetSenderId());
	Serial.println();
	ConfirmationMessage confirmation { message.GetType(), message.GetSenderId(), message.GetMessageId() };
	LoRaVoting.SendMessage(confirmation);
}
