#include <LoRaVoting.h>
#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
	Serial.begin(9600);                   // initialize serial
	LoRaVoting.init();
}

void loop() {
  // put your main code here, to run repeatedly:
	String content("a content example 23 abc");
	BroadcastMessage message{ RegisteredDevices, content};
	LoRaVoting.SendMessage(message);
	delay(2000);
}
