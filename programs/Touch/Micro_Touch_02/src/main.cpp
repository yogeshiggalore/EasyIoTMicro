/*******************************************************************************
* Project Name: Micro_Touch_02
*
* Version: 1.0
*
* Description:
* This project test touch inputs
*
*
* Owner
* Yogesh M Iggalore
********************************************************************************
* Copyright (2021-22) EasyIoT
*******************************************************************************/

#include <Arduino.h>

/* define touch and led pins */
#define TOUCH_INPUT_1	15
#define TOUCH_INPUT_2	12
#define TOUCH_INPUT_3	14
#define TOUCH_INPUT_4	27

#define GREEN_LED_PIN	2
#define RED_LED_PIN		4
#define BLUE_LED_PIN	13

void IRAM_ATTR outside_interrupt_handler_t1(void);
void IRAM_ATTR outside_interrupt_handler_t2(void);
void IRAM_ATTR outside_interrupt_handler_t3(void);
void IRAM_ATTR outside_interrupt_handler_t4(void);

unsigned long ulTouchClearTimeout=0;

void setup() {

	/* set green,red and blue led pins as outputs */
	pinMode(GREEN_LED_PIN,OUTPUT);
	pinMode(RED_LED_PIN,OUTPUT);
	pinMode(BLUE_LED_PIN,OUTPUT);

	touchAttachInterrupt(TOUCH_INPUT_1, outside_interrupt_handler_t1, 20);
	touchAttachInterrupt(TOUCH_INPUT_2, outside_interrupt_handler_t2, 20);
	touchAttachInterrupt(TOUCH_INPUT_3, outside_interrupt_handler_t3, 20);
	touchAttachInterrupt(TOUCH_INPUT_4, outside_interrupt_handler_t4, 20);

	ulTouchClearTimeout = millis();
}

void loop() {

	if((millis() - ulTouchClearTimeout) > 500){
		ulTouchClearTimeout = millis();
		digitalWrite(GREEN_LED_PIN,LOW);
		digitalWrite(RED_LED_PIN,LOW);
		digitalWrite(BLUE_LED_PIN,LOW);
	}
}

void IRAM_ATTR outside_interrupt_handler_t1(void) { 
	digitalWrite(GREEN_LED_PIN,HIGH);
	ulTouchClearTimeout = millis();
}

void IRAM_ATTR outside_interrupt_handler_t2(void) {
  	digitalWrite(RED_LED_PIN,HIGH);
	ulTouchClearTimeout = millis();
}

void IRAM_ATTR outside_interrupt_handler_t3(void) {
	digitalWrite(BLUE_LED_PIN,HIGH);
   	ulTouchClearTimeout = millis();
}

void IRAM_ATTR outside_interrupt_handler_t4(void) {
	digitalWrite(GREEN_LED_PIN,HIGH);
	digitalWrite(RED_LED_PIN,HIGH);
	digitalWrite(BLUE_LED_PIN,HIGH);
	ulTouchClearTimeout = millis();	
}