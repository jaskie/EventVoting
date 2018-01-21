#include <LoRaVoting.h>
#include <Arduino.h>
#include <eeprom.h>
//#include <U8g2lib.h>

#define BUTTON_COUNT 4

const int buttonPins[BUTTON_COUNT] = { 14, 15, 16, 17};
bool buttonStates[BUTTON_COUNT];
ResponseMessage * responseToSend = nullptr;
byte responseRetryCount = 0;

//U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ 8);  // Nokia 5110 Display

void setup(void) {

  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  Serial.println("Keypad initializing");
  
  EEPROM.begin();

  //u8g2.begin();
  //u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function
  for (size_t x = 0; x < BUTTON_COUNT; x++)
  {
	  pinMode(buttonPins[x], INPUT_PULLUP);
	  buttonStates[x] = digitalRead(buttonPins[x]);
  }
  LoRaVoting.init();
  LoRaVoting.ReceivedBroadcastCallback(onReceiveBroadcast);
  LoRaVoting.ReceivedConfirmationCallback(onConfirmation);

  if (LoRaVoting.IsReady())
	Serial.println("LoRaVoting ready");
}

void loop(void) {
	for (size_t x = 0; x < BUTTON_COUNT; x++)
	{
		bool newState = digitalRead(buttonPins[x]);
		if (newState != buttonStates[x])
		{
			buttonStates[x] = newState;
			Serial.println("Button " + String(x) + " state changed to " + String(newState));
		}
	}
	delay(500);
	retrySendingResponse();
}

void onReceiveBroadcast(const BroadcastMessage& message) {
	if (responseToSend)
	{
		Serial.println("previous response not deleted");
		delete responseToSend;
	}
	Serial.print("received type: ");
	Serial.println(message.GetType());
	switch (message.GetType())
	{
	case RegisteredDevices:
		replyRegisteredDevices();
		break;
	case NewDevice:
		Serial.println(message.GetContent());
		newDevice(message.GetContent());
		break;
	case DropDevice:
		clearDeviceId();
		break;
	case StartVoting:
		break;
	case EndVoting:
		break;
	case Display:
		break;
	default:
		break;
	}

	/*Serial.print("Received: ");
	Serial.print(message.GetContent());
	Serial.println();
	const byte response[]{ 12u, 32u };
	responseToSend = new ResponseMessage(message.GetType(), deviceId, response, 2);
	LoRaVoting.SendMessage(*responseToSend);*/
  /*
  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();

  u8g2.setFont(u8g2_font_pxplusibmvga8_m_all);  
  u8g2.setFontDirection(0);
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print(incomingMsgId);
  u8g2.setCursor(0, 30);
  u8g2.print(incoming);
  u8g2.setCursor(0, 45);
  u8g2.setFont(u8g2_font_blipfest_07_tr);  
  u8g2.println("RSSI: " + String(LoRa.packetRssi()));
  u8g2.setCursor(45, 45);
  u8g2.println("Snr: " + String(LoRa.packetSnr()));
  u8g2.sendBuffer();
  */
}

void readDeviceId(byte deviceId[DEVICE_ID_LENGTH])
{
	for (size_t i = 0; i < DEVICE_ID_LENGTH; i++)
		deviceId[i] = EEPROM.read(i);
}

void storeDeviceId(byte deviceId[DEVICE_ID_LENGTH])
{
	for (size_t i = 0; i < DEVICE_ID_LENGTH; i++)
		EEPROM.update(i, deviceId[i]);
}

void clearDeviceId()
{
	for (size_t i = 0; i < DEVICE_ID_LENGTH; i++)
		EEPROM.update(i, 0);
}

bool haveEmptyDeviceId()
{
	for (size_t i = 0; i < DEVICE_ID_LENGTH; i++)
		if (EEPROM.read(i))
			return false;
	return true;
}

void newDevice(const String& deviceIdStr)
{
	Serial.println("asked to add new device");

	if (!haveEmptyDeviceId())
		return;
	Serial.println("began to program device id");
	byte deviceId[DEVICE_ID_LENGTH + 1];//to fit \0
	deviceIdStr.getBytes((unsigned char*)deviceId, DEVICE_ID_LENGTH + 1);
	storeDeviceId(deviceId);
}

void onConfirmation(const ConfirmationMessage& message) 
{
	if (responseToSend && message.IsConfirmationTo(responseToSend->GetType(), responseToSend->GetMessageId(), responseToSend->GetSenderId()))
	{
		Serial.println("Received confirmation.");
		delete(responseToSend);
		responseToSend = nullptr;
	}
}

void retrySendingResponse()
{
	if (responseToSend)
	{
		Serial.println("sending...");
			LoRaVoting.SendMessage(*responseToSend);
	}
}

void replyRegisteredDevices()
{
	Serial.println("Started replying.");
	byte deviceId[DEVICE_ID_LENGTH];
	readDeviceId(deviceId);
	responseToSend = new ResponseMessage(RegisteredDevices, deviceId, NULL, 0);
	retrySendingResponse();
}

