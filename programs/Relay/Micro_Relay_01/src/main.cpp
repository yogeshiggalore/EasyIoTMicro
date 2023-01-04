/*******************************************************************************
* Project Name:Micro_Relay_01
*
* Version: 1.0
*
* Description:
* In this project esp32 control relay on/off
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoT
*******************************************************************************/
#include <Arduino.h>

#define RELAY_PIN	25

void setup() {
	/* set relay pin as output*/
	pinMode(RELAY_PIN,OUTPUT);
}

void loop() {

	digitalWrite(RELAY_PIN, HIGH);
	delay(5000);
	digitalWrite(RELAY_PIN, LOW);
	delay(5000);
}