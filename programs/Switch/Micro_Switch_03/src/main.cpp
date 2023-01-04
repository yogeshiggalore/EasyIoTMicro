/*******************************************************************************
* Project Name: Micro_Switch_03
*
* Version: 1.0
*
* Description:
* This project, change led state on switch press 
* 
*
* Owner:
* Yogesh M Iggalore
********************************************************************************
* Copyright (2021-22) , EasyIoTMicro
*******************************************************************************/
#include <Arduino.h>

#define SWITCH_1_PIN	34
#define GREEN_LED_PIN	2

bool bLEDState=false;

void setup() {
	/* set SWITCH_1_PIN as input */
	pinMode(SWITCH_1_PIN, INPUT);

	/*set GREEN_LED_PIN as output */
	pinMode(GREEN_LED_PIN, OUTPUT);

	/* turn off led at begin */
	digitalWrite(GREEN_LED_PIN,LOW);
	bLEDState = false;
}

void loop() {
	if(digitalRead(SWITCH_1_PIN) == 0){
		if(bLEDState){
			digitalWrite(GREEN_LED_PIN,LOW);
			bLEDState = false;
		}else{
			digitalWrite(GREEN_LED_PIN,HIGH);
			bLEDState = true;
		}
	}
}

/* isr callback function */
void IRAM_ATTR isr_switch_1_press(void){
	if(bLEDState){
		digitalWrite(GREEN_LED_PIN,LOW);
		bLEDState = false;
	}else{
		digitalWrite(GREEN_LED_PIN,HIGH);
		bLEDState = true;
	}
}