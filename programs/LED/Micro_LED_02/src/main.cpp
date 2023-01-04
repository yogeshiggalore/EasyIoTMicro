/*******************************************************************************
* Project Name: Micro_LED_02
*
* Version: 1.0
*
* Description:
* In this project esp32, switches leds on/off using firmware delay.
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2021-22) , EasyIoT
*******************************************************************************/

/* include header files */
#include <Arduino.h>

#define GREE_LED_PIN 	2
#define BLUE_LED_PIN 	13

void setup(){
	/* set LED pins to output*/
	pinMode(GREE_LED_PIN, OUTPUT);
	pinMode(BLUE_LED_PIN, OUTPUT);
}

void loop(){
	
	/* turn on LED pins */
	digitalWrite(GREE_LED_PIN,HIGH);
	digitalWrite(BLUE_LED_PIN,LOW);

	/* wait for 500 milisecond */
	delay(500);

	/* turn off LED pin */
	digitalWrite(GREE_LED_PIN,LOW);
	digitalWrite(BLUE_LED_PIN,HIGH);

	/* wait for 500 milisecond */
	delay(500);

}