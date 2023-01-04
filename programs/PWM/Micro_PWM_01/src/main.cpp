/*******************************************************************************
* Project Name:Micro_PWM_01
*
* Version: 1.0
*
* Description:
* In this project esp32 uses PWM to control brightness of led
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
	
	/* set dutycyle to 0% */
	ui8DutyCycle = map(0,0,100,0,255);
	ledcWrite(PWM_CHANNEL,ui8DutyCycle);
	delay(1000);

	/* set dutycyle to 25% */
	ui8DutyCycle = map(25,0,100,0,255);
	ledcWrite(PWM_CHANNEL,ui8DutyCycle);
	delay(1000);

	/* set dutycyle to 50% */
	ui8DutyCycle = map(50,0,100,0,255);
	ledcWrite(PWM_CHANNEL,ui8DutyCycle);
	delay(1000);

	/* set dutycyle to 75% */
	ui8DutyCycle = map(75,0,100,0,255);
	ledcWrite(PWM_CHANNEL,ui8DutyCycle);
	delay(1000);

	/* set dutycyle to 100% */
	ui8DutyCycle = map(100,0,100,0,255);
	ledcWrite(PWM_CHANNEL,ui8DutyCycle);
	delay(1000);

}