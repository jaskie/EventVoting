#include <LoRaVoting.h>
#include <Arduino.h>
#include <rBase64.h>

#define HOST_BUFFER_SIZE 0x100

static char serial_buffer[HOST_BUFFER_SIZE];
static char message_buffer[HOST_BUFFER_SIZE];

#define REGISTERED_DEVICES_QUERY	0x01
#define REGISTER_NEW_DEVICE			0x02
#define DROP_DEVICE					0x03 
#define START_VOTING				0x04
#define END_VOTING					0x05
#define DISPLAY_MESSAGE				0x06

void setup() {
  // put your setup code here, to run once:
	Serial.setTimeout(10);
	Serial.begin(115200);                   // initialize serial
	LoRaVoting.init();
	LoRaVoting.ReceivedResponseCallback(onReceiveResponse);
	if (LoRaVoting.IsReady())
		confirmToHost();
}

void loop() {
  // put your main code here, to run repeatedly:
	//LoRaVoting.SendMessage(BroadcastMessage(RegisteredDevices, "asking for registered devices"));
	if (Serial.available())
	{
		size_t bytes_readed = Serial.readBytesUntil('\n', serial_buffer, HOST_BUFFER_SIZE);
		if (bytes_readed > 0)
		{
			size_t decoded_length = rbase64_decode(message_buffer, serial_buffer, bytes_readed);
			if (decoded_length > 0 && decoded_length < HOST_BUFFER_SIZE - 1)
			{
				message_buffer[decoded_length] = 0; // to easily convert tyhe content to String
				parseHostMessage(message_buffer, decoded_length);
			}
		}
	}
	//LoRaVoting.SendMessage(BroadcastMessage(NewDevice, "ABCDEFGHIJKLMNOP"));
}


void onReceiveResponse(const ResponseMessage& message) 
{
	ConfirmationMessage confirmation { message.GetType(), message.GetSenderId(), message.GetMessageId() };
	LoRaVoting.SendMessage(confirmation);
	sendResponseToHost(message);
}


void parseHostMessage(char buffer[], size_t buffer_size)
{
	switch (buffer[0])
	{
	case REGISTERED_DEVICES_QUERY:
		LoRaVoting.SendMessage(BroadcastMessage(RegisteredDevices, buffer + 1, buffer_size - 1));
		confirmToHost();
		break;
	case REGISTER_NEW_DEVICE:
		LoRaVoting.SendMessage(BroadcastMessage(NewDevice, buffer + 1, buffer_size - 1));
		confirmToHost();
		break;
	case DROP_DEVICE:
		LoRaVoting.SendMessage(BroadcastMessage(DropDevice, buffer + 1, buffer_size - 1));
		confirmToHost();
		break;
	case START_VOTING:
		LoRaVoting.SendMessage(BroadcastMessage(StartVoting, buffer + 1, buffer_size - 1));
		confirmToHost();
		break;
	case END_VOTING:
		LoRaVoting.SendMessage(BroadcastMessage(EndVoting, buffer + 1, buffer_size - 1));
		confirmToHost();
		break;
	case DISPLAY_MESSAGE:
		LoRaVoting.SendMessage(BroadcastMessage(Display, buffer + 1, buffer_size - 1));
		confirmToHost();
		break;
	default:
		break;
	}
}

void confirmToHost()
{
	sendToHost("\0OK", 3);
}

void sendToHost(char* buffer, byte size)
{
	size_t length = rbase64_encode(serial_buffer, buffer, size);
	serial_buffer[length] = '\n'; 
	Serial.write(serial_buffer, length + 1);
}

void sendResponseToHost(const ResponseMessage& message)
{
	byte length = 0;
	message_buffer[length++] = message.GetType();
	memcpy(message_buffer + length, message.GetSenderId(), DEVICE_ID_LENGTH);
	length += DEVICE_ID_LENGTH;
	message_buffer[length++] = message.GetContentLength();
	memcpy(message_buffer + length, message.GetContent(), message.GetContentLength());
	length += message.GetContentLength();
	sendToHost(message_buffer, length);
}