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
	String content("a content example poszedł 23 abc");
	BroadcastMessage message { RegisteredDevices, content};
	LoRaVoting.SendMessage(message);
	delay(2000);
}


void onReceiveResponse(const ResponseMessage& message) 
{
	Serial.print("received response: ");
	Serial.print(message.GetContent()[0]);
	Serial.print(message.GetContent()[1]);
	Serial.print(" from ");
	Serial.print((char*)message.GetSenderId());
	Serial.println();
}
