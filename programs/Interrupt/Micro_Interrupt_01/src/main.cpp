/*******************************************************************************
* Project Name: Micro_Interrupt_01
*
* Version: 1.0
*
* Description:
* This project, turn on LED based on Interrupt switch pressed 
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

/* isr function for switch_1 press */
void IRAM_ATTR isr_switch_1_press(void);

bool bLEDState=false;

void setup() {
	/* set SWITCH_1_PIN as input */
	pinMode(SWITCH_1_PIN, INPUT);

	/*set GREEN_LED_PIN as output */
	pinMode(GREEN_LED_PIN, OUTPUT);

	/* turn off led at begin */
	digitalWrite(GREEN_LED_PIN,LOW);
	bLEDState = false;

	/* attaching interrupt to switch_1 */
  	attachInterrupt(SWITCH_1_PIN, isr_switch_1_press, FALLING);
}

void loop() {

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