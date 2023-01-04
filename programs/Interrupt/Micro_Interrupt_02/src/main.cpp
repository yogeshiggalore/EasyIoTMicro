/*******************************************************************************
* Project Name: Micro_Interrupt_02
*
* Version: 1.0
*
* Description:
* This project, turn on LEDs based on Interrupt switches pressed 
* 
*
* Owner:
* Yogesh M Iggalore
********************************************************************************
* Copyright (2021-22) , EasyIoTMicro
*******************************************************************************/
#include <Arduino.h>

#define SWITCH_1_PIN	34
#define SWITCH_2_PIN	35
#define SWITCH_3_PIN	39

#define GREEN_LED_PIN	2
#define RED_LED_PIN		4
#define BLUE_LED_PIN	13

/* isr function for switches */
void IRAM_ATTR isr_switch_1_press(void);
void IRAM_ATTR isr_switch_2_press(void);
void IRAM_ATTR isr_switch_3_press(void);

bool bGreenLEDState=false;
bool bRedLEDState=false;
bool bBlueLEDState=false;

void setup() {
	/* set switches as input */
	pinMode(SWITCH_1_PIN, INPUT);
	pinMode(SWITCH_2_PIN, INPUT);
	pinMode(SWITCH_3_PIN, INPUT);

	/*set LEDs as output */
	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(RED_LED_PIN, OUTPUT);
	pinMode(BLUE_LED_PIN, OUTPUT);

	/* turn off led at begin */
	digitalWrite(GREEN_LED_PIN,LOW);
	digitalWrite(RED_LED_PIN,LOW);
	digitalWrite(BLUE_LED_PIN,LOW);

	bGreenLEDState = false;
	bRedLEDState = false;
	bBlueLEDState = false;

	/* attaching interrupt to switches */
  	attachInterrupt(SWITCH_1_PIN, isr_switch_1_press, FALLING);
	attachInterrupt(SWITCH_2_PIN, isr_switch_2_press, FALLING);
	attachInterrupt(SWITCH_3_PIN, isr_switch_3_press, FALLING);
}

void loop() {

}

/* isr callback function */
void IRAM_ATTR isr_switch_1_press(void){
	digitalWrite(RED_LED_PIN,LOW);
	digitalWrite(BLUE_LED_PIN,LOW);
	if(bGreenLEDState){
		digitalWrite(GREEN_LED_PIN,LOW);
		bGreenLEDState = false;
	}else{
		digitalWrite(GREEN_LED_PIN,HIGH);
		bGreenLEDState = true;
	}
}

void IRAM_ATTR isr_switch_2_press(void){
	digitalWrite(GREEN_LED_PIN,LOW);
	digitalWrite(BLUE_LED_PIN,LOW);
	if(bRedLEDState){
		digitalWrite(RED_LED_PIN,LOW);
		bRedLEDState = false;
	}else{
		digitalWrite(RED_LED_PIN,HIGH);
		bRedLEDState = true;
	}
}

void IRAM_ATTR isr_switch_3_press(void){
	digitalWrite(RED_LED_PIN,LOW);
	digitalWrite(GREEN_LED_PIN,LOW);
	if(bBlueLEDState){
		digitalWrite(BLUE_LED_PIN,LOW);
		bBlueLEDState = false;
	}else{
		digitalWrite(BLUE_LED_PIN,HIGH);
		bBlueLEDState = true;
	}
}