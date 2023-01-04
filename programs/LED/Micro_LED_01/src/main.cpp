/*******************************************************************************
* Project Name: Micro_LED_01
*
* Version: 1.0
*
* Description:
* In this project esp32, switches led on/off using firmware delay.
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2021-22) , EasyIoT
*******************************************************************************/

/* include header files */
#include <Arduino.h>

#define GREEN_LED_PIN 2

void setup(){
	/* set LED pin to output*/
	pinMode(GREEN_LED_PIN, OUTPUT);
}

void loop(){
	
	/* turn on LED pin */
	digitalWrite(GREEN_LED_PIN,HIGH);

	/* wait for 500 milisecond */
	delay(500);

	/* turn off LED pin */
	digitalWrite(GREEN_LED_PIN,LOW);

	/* wait for 500 milisecond */
	delay(500);

}