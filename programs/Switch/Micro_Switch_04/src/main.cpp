/*******************************************************************************
* Project Name: Micro_Switch_04
*
* Version: 1.0
*
* Description:
* This project, change led state on switch press with debounce delay
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

#define DEBOUCE_TIME	50

bool bLEDState=false;
bool bProcessedSwitchState=false;
bool bCurrentSwitchState=false;
bool bLastSwitchState=false;

unsigned long ulLastDebounceTime = 0;

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
	/* read current state of switch */
	bCurrentSwitchState  = digitalRead(SWITCH_1_PIN);

	/* if current switch state is not equal to last state then update debounce time */
	if(bCurrentSwitchState != bLastSwitchState){
		ulLastDebounceTime = millis();
	}

	/* if current state is greater than DEBOUNCE_TIME check processed switch state */
	if((millis() - ulLastDebounceTime) > DEBOUCE_TIME){
		if(bCurrentSwitchState != bProcessedSwitchState){
			bProcessedSwitchState = bCurrentSwitchState;
			/* if bProcessedSwitchState is low means switch is pressed more than DEBOUCE_TIME toggle led state*/
			if(bProcessedSwitchState == LOW){
				if(bLEDState){
					digitalWrite(GREEN_LED_PIN,LOW);
					bLEDState = false;
				}else{
					digitalWrite(GREEN_LED_PIN,HIGH);
					bLEDState = true;
				}
			}
		}
	}

	/* update current switch state to last switch state */
	bLastSwitchState = bCurrentSwitchState;
}
