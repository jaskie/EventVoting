#include "RadioMessage.h"
#include "Radio.h"
#include <Arduino.h>
#include <U8g2lib.h>
#include "Radio.h"

const int buttonPins[] = { 14, 15, 16, 17};
bool buttonStates[4];

U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ 8);  // Nokia 5110 Display

void setup(void) {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  Serial.println("Keypad");

  u8g2.begin();
  u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function
  Serial.println("LoRa init succeeded.");
  for (int x = 0; x < sizeof(buttonPins); x++)
  {
	  pinMode(buttonPins[x], INPUT_PULLUP);
	  buttonStates[x] = digitalRead(buttonPins[x]);
  }
}

void loop(void) {
	for (int x = 0; x < sizeof(buttonPins); x++)
	{
		bool newState = digitalRead(buttonPins[x]);
		if (newState != buttonStates[x])
		{
			buttonStates[x] = newState;
			Serial.println("Button " + String(x) + " state changed to " + String(newState));
		}
	}
	delay(100);
}


void onReceive(int packetSize) {

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

