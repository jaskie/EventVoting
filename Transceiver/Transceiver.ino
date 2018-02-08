#include <LoRaVoting.h>
#include <Arduino.h>
#include <rBase64.h>

#define BUFFER_SIZE 256
byte serial_buffer[BUFFER_SIZE];
byte message_buffer[BUFFER_SIZE];

void setup() {
  // put your setup code here, to run once:
	Serial.begin(115200);                   // initialize serial
	LoRaVoting.init();
	LoRaVoting.ReceivedResponseCallback(onReceiveResponse);
	if (LoRaVoting.IsReady())
		Serial.println("LoRaVoting transceiver ready");
}

void loop() {
  // put your main code here, to run repeatedly:
	LoRaVoting.SendMessage(BroadcastMessage(RegisteredDevices, "asking for registered devices"));
	if (Serial.available())
	{
		size_t bytes_readed = Serial.readBytesUntil('\0', serial_buffer, BUFFER_SIZE);
		size_t decoded_length = rbase64_decode((char*)serial_buffer, (char*)message_buffer, bytes_readed);

	}
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
