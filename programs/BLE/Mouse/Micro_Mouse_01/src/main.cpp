/*******************************************************************************
* Project Name:Micro_Mouse_01
*
* Version: 1.0
*
* Description:
* In this project esp32 used as BLE mouse
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2021-22) , EasyIoTMicro
*******************************************************************************/
#include <Arduino.h>
#include <BleMouse.h>

#define SWITCH_1_PIN	34
#define SWITCH_2_PIN	35
#define SWITCH_3_PIN	39
#define SWITCH_4_PIN	36

#define SW1		0
#define SW2		1
#define SW3		2
#define SW4		3

#define GREEN_LED_PIN	2
#define RED_LED_PIN		4
#define BLUE_LED_PIN	13

#define DEBOUCE_TIME	50

bool bLEDState[4];
bool bProcessedSwitchState[4];
bool bCurrentSwitchState[4];
bool bLastSwitchState[4];
unsigned long ulLastDebounceTime[4];

/* ble mouse */
BleMouse bleMouse;

void read_switch(uint8_t ui8Switch);

void setup() {
	delay(1000);
	
	/* start ble mouse */
	bleMouse.begin();
  	
	/* start serial */
	Serial.begin(921600);

	Serial.println("BLE mouse demo ");

	/* set switches as input */
	pinMode(SWITCH_1_PIN, INPUT);
	pinMode(SWITCH_2_PIN, INPUT);
	pinMode(SWITCH_3_PIN, INPUT);
	pinMode(SWITCH_4_PIN, INPUT);

	/* set leds as output */
	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(RED_LED_PIN, OUTPUT);
	pinMode(BLUE_LED_PIN, OUTPUT);

	/* turn off led at begin */
	digitalWrite(GREEN_LED_PIN,LOW);
	digitalWrite(RED_LED_PIN,LOW);
	digitalWrite(BLUE_LED_PIN,LOW);
}

void loop() {
	/* if mouse connected then read all switches */
	if(bleMouse.isConnected()){
		read_switch(SW1);
		read_switch(SW2);
		read_switch(SW3);
		read_switch(SW4);
	}
}

void read_switch(uint8_t ui8Switch){

	if(ui8Switch == SW1){
		/* read current state of switch */
		bCurrentSwitchState[SW1]  = digitalRead(SWITCH_1_PIN);
	}else if(ui8Switch == SW2){
		/* read current state of switch */
		bCurrentSwitchState[SW2]  = digitalRead(SWITCH_2_PIN);
	}else if(ui8Switch == SW3){
		/* read current state of switch */
		bCurrentSwitchState[SW3]  = digitalRead(SWITCH_3_PIN);
	}else if(ui8Switch == SW4){
		/* read current state of switch */
		bCurrentSwitchState[SW4]  = digitalRead(SWITCH_4_PIN);
	}else{

	}
	
	/* if current switch state is not equal to last state then update debounce time */
	if(bCurrentSwitchState[ui8Switch] != bLastSwitchState[ui8Switch]){
		ulLastDebounceTime[ui8Switch] = millis();
	}

	/* if current state is greater than DEBOUNCE_TIME check processed switch state */
	if((millis() - ulLastDebounceTime[ui8Switch]) > DEBOUCE_TIME){
		if(bCurrentSwitchState[ui8Switch] != bProcessedSwitchState[ui8Switch]){
			bProcessedSwitchState[ui8Switch] = bCurrentSwitchState[ui8Switch];
			/* if bProcessedSwitchState is low means switch is pressed more than DEBOUCE_TIME toggle led state*/
			if(bProcessedSwitchState[ui8Switch] == LOW){
				if(ui8Switch == SW1){
					bleMouse.click(MOUSE_RIGHT);
					Serial.println("Right click ");
					digitalWrite(RED_LED_PIN,LOW);
					digitalWrite(BLUE_LED_PIN,LOW);
					digitalWrite(GREEN_LED_PIN,HIGH);
				}else if(ui8Switch == SW2){
					bleMouse.click(MOUSE_LEFT);
					Serial.println("Left click");
					digitalWrite(RED_LED_PIN,HIGH);
					digitalWrite(BLUE_LED_PIN,LOW);
					digitalWrite(GREEN_LED_PIN,LOW);
				}else if(ui8Switch == SW3){
					bleMouse.click(MOUSE_FORWARD);
					Serial.println("Forward click ");
					digitalWrite(RED_LED_PIN,LOW);
					digitalWrite(BLUE_LED_PIN,HIGH);
					digitalWrite(GREEN_LED_PIN,LOW);
				}else if(ui8Switch == SW4){
					bleMouse.click(MOUSE_BACK);
					Serial.println("back click");
					digitalWrite(RED_LED_PIN,LOW);
					digitalWrite(BLUE_LED_PIN,LOW);
					digitalWrite(GREEN_LED_PIN,LOW);
				}else{

				}
			}
		}
	}

	/* update current switch state to last switch state */
	bLastSwitchState[ui8Switch] = bCurrentSwitchState[ui8Switch];
}