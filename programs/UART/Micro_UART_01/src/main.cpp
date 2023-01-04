/*******************************************************************************
* Project Name: Micro_UART_01
*
* Version: 1.0
*
* Description:
* In this project esp32, UART example
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

	/* start UART0 */
	Serial.begin(9600);

	Serial.println("Project Micro_UART_01 started");
}

void loop(){
	
	/* turn on LED pin */
	digitalWrite(GREEN_LED_PIN,HIGH);

	/* print message to serial */
	Serial.println("Green LED on");

	/* wait for 1000 milisecond */
	delay(1000);

	/* turn off LED pin */
	digitalWrite(GREEN_LED_PIN,LOW);

	/* print message to serial */
	Serial.println("Green LED off");

	/* wait for 1000 milisecond */
	delay(1000);

}