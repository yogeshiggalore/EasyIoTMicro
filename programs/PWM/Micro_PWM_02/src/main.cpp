/*******************************************************************************
* Project Name:Micro_PWM_02
*
* Version: 1.0
*
* Description:
* In this project esp32 uses PWM to led fadein fadeout
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2021-22) , EasyIoT
*******************************************************************************/

/* include headers */
#include <Arduino.h>

/* define led pin */
#define  GREEN_LED_PIN	2

/* set pwm frequency,channel and resolution */
#define PWM_FREQUENCY	5000
#define PWM_CHANNEL		0
#define PWM_RESOLUTION	8

uint8_t ui8DutyCycle;

void setup(){

	/* configure PWM functionalities */
	ledcSetup(PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION);

	/* attach led pin to PWM */
	ledcAttachPin(GREEN_LED_PIN,PWM_CHANNEL);

}

void loop(){
	
	/* fadein */
	for(ui8DutyCycle=0;ui8DutyCycle<255;ui8DutyCycle++){
		ledcWrite(PWM_CHANNEL,ui8DutyCycle);
		delay(15);
	}
	
	/* fadeout */
	for(ui8DutyCycle=255;ui8DutyCycle>0;ui8DutyCycle--){
		ledcWrite(PWM_CHANNEL,ui8DutyCycle);
		delay(15);
	}
}