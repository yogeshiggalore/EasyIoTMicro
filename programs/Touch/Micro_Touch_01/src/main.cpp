/*******************************************************************************
* Project Name: Micro_Touch_01
*
* Version: 1.0
*
* Description:
* This project test touch input
*
*
* Owner
* Yogesh M Iggalore
********************************************************************************
* Copyright (2021-22) EasyIot
*******************************************************************************/

#include <Arduino.h>
#include <Arduino.h>

/* define touch and led pins */
#define TOUCH_INPUT_1	15
#define GREEN_LED_PIN	2

void setup() {

	/* set green led pin as output */
	pinMode(GREEN_LED_PIN,OUTPUT);

}

void loop() {

	if(touchRead(TOUCH_INPUT_1) < 55){
		digitalWrite(GREEN_LED_PIN,HIGH);
	}else{
		digitalWrite(GREEN_LED_PIN,LOW);
	}
}