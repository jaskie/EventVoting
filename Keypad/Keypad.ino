#include <LoRaVoting.h>
#include <Arduino.h>
#include <eeprom.h>
#include <U8g2lib.h>

#define BUTTON_COUNT 4

const int buttonPins[BUTTON_COUNT] = { 14, 15, 16, 17};
bool buttonStates[BUTTON_COUNT];
ResponseMessage * responseToSend = nullptr;
byte responseRetryCount = 0;
bool isVoting = false;
unsigned long lastSendResponseRetry;


U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ 8);  // Nokia 5110 Display

void setup(void) {

  Serial.begin(115200);                   // initialize serial
  while (!Serial);
  Serial.println("Keypad initializing");
  
  EEPROM.begin();

  u8g2.begin();
  u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function
  for (size_t x = 0; x < BUTTON_COUNT; x++)
  {
	  pinMode(buttonPins[x], INPUT_PULLUP);
	  buttonStates[x] = digitalRead(buttonPins[x]);
  }
  LoRaVoting.init();
  LoRaVoting.ReceivedBroadcastCallback(onReceiveBroadcast);
  LoRaVoting.ReceivedConfirmationCallback(onConfirmation);
  lastSendResponseRetry = millis();

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
			if (isVoting && newState == 0)
			{
				replyWithVote(x + 1);
				isVoting = false;
			}
			Serial.println("Button " + String(x) + " state changed to " + String(newState));
		}
	}
	if (millis() > lastSendResponseRetry + 500)
	{
		lastSendResponseRetry = millis();
		retrySendingResponse();
	}
}

void onReceiveBroadcast(const BroadcastMessage& message) {
	if (responseToSend)
	{
		Serial.println("previous response not deleted");
		delete responseToSend;
	}
	Serial.print("received type: ");
	Serial.print(message.GetType());
	Serial.print(" length: ");
	Serial.println(message.GetContentLength());

	switch (message.GetType())
	{
	case RegisteredDevices:
		replyRegisteredDevices();
		break;
	case NewDevice:
		newDevice(message.GetContent());
		break;
	case DropDevice:
		clearDeviceId();
		break;
	case StartVoting:
		isVoting = true;
		lcdPrintBytes(message.GetContent(), message.GetContentLength());
		break;
	case EndVoting:
		isVoting = false;
		lcdPrintBytes(message.GetContent(), message.GetContentLength());
		break;
	case Display:
		lcdPrintBytes(message.GetContent(), message.GetContentLength());
		break;
	default:
		break;
	}
}

void lcdPrintBytes(const byte* content, const byte contentLength)
{
	u8g2.setFont(u8g2_font_pxplusibmvga8_m_all);
	u8g2.setFontDirection(0);
	u8g2.clearBuffer();
	u8g2.setCursor(0, 15);
	u8g2.print((char*)content);
	u8g2.sendBuffer();
}

void lcdPrintString(const String& content)
{
	u8g2.setFont(u8g2_font_pxplusibmvga8_m_all);
	u8g2.setFontDirection(0);
	u8g2.clearBuffer();
	u8g2.setCursor(0, 15);
	u8g2.print(content);
	u8g2.sendBuffer();
}

void readDeviceId(byte deviceId[DEVICE_ID_LENGTH])
{
	for (size_t i = 0; i < DEVICE_ID_LENGTH; i++)
		deviceId[i] = EEPROM.read(i);
}

void storeDeviceId(const byte deviceId[DEVICE_ID_LENGTH])
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

void newDevice(const byte* deviceId)
{
	Serial.println("asked to add new device");

	if (!haveEmptyDeviceId())
		return;
	Serial.println("began to program device id");
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


void replyWithVote(const byte& value)
{
	Serial.println("Started voting.");
	byte deviceId[DEVICE_ID_LENGTH];
	readDeviceId(deviceId);
	responseToSend = new ResponseMessage(StartVoting, deviceId, &value, 1);
	retrySendingResponse();
	lcdPrintString(String(value));
}

