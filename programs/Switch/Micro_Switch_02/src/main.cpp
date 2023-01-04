/*******************************************************************************
* Project Name: Micro_Switch_02
*
* Version: 1.0
*
* Description:
* This project, turn on LEDs based on switches pressed 
* 
*
* Owner:
* Yogesh M Iggalore
********************************************************************************
* Copyright (2021-22) , EasyIoTMicro
*******************************************************************************/

#include <Arduino.h>

/* define LED1 and switch pins */
#define GREEN_LED_PIN	2
#define RED_LED_PIN		4
#define BLUE_LED_PIN	13

#define SWITCH_1_PIN		34
#define SWITCH_2_PIN		35
#define SWITCH_3_PIN		39

void setup(){

	/* set LED1 pin as output */
	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(RED_LED_PIN, OUTPUT);
	pinMode(BLUE_LED_PIN, OUTPUT);

	/* set switch pin as input */
	pinMode(SWITCH_1_PIN, INPUT);
	pinMode(SWITCH_2_PIN, INPUT);
	pinMode(SWITCH_3_PIN, INPUT);
}

void loop() {

	/* read switch1 input and control green led */
	if(digitalRead(SWITCH_1_PIN)){
		/* turn off LED */
		digitalWrite(GREEN_LED_PIN, LOW);
	}else{
		/* turn on LED */
		digitalWrite(GREEN_LED_PIN, HIGH);
	}

	/* read switch2 input and control red led */
	if(digitalRead(SWITCH_2_PIN)){
		/* turn off LED */
		digitalWrite(RED_LED_PIN, LOW);
	}else{
		/* turn on LED */
		digitalWrite(RED_LED_PIN, HIGH);
	}

	/* read switch3 input and control blue led */
	if(digitalRead(SWITCH_3_PIN)){
		/* turn off LED */
		digitalWrite(BLUE_LED_PIN, LOW);
	}else{
		/* turn on LED */
		digitalWrite(BLUE_LED_PIN, HIGH);
	}
}
