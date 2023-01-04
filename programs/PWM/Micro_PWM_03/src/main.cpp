/*******************************************************************************
* Project Name:Micro_PWM_03
*
* Version: 1.0
*
* Description:
* In this project esp32 uses PWM to led fadein fadeout
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2021-22) , EasyIoTMicro
*******************************************************************************/

/* include headers */
#include <Arduino.h>

/* define led pin */
#define  GREEN_LED_PIN	2
#define  RED_LED_PIN	4
#define  BLUE_LED_PIN	13

/* set pwm frequency,channel and resolution */
#define PWM_FREQUENCY	5000

#define GREEN_PWM_CHANNEL	0
#define RED_PWM_CHANNEL		1
#define BLUE_PWM_CHANNEL	2

#define PWM_RESOLUTION	8

uint8_t ui8DutyCycle;

void setup(){

	/* configure PWM functionalities */
	ledcSetup(GREEN_PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION);
	ledcSetup(RED_PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION);
	ledcSetup(BLUE_PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION);

	/* attach led pin to PWM */
	ledcAttachPin(GREEN_LED_PIN,GREEN_PWM_CHANNEL);
	ledcAttachPin(RED_LED_PIN,RED_PWM_CHANNEL);
	ledcAttachPin(BLUE_LED_PIN,BLUE_PWM_CHANNEL);
}

void loop(){
	ui8DutyCycle = map(0,0,100,0,255);
	ledcWrite(GREEN_PWM_CHANNEL,ui8DutyCycle);
	ledcWrite(RED_PWM_CHANNEL,ui8DutyCycle);
	ledcWrite(BLUE_PWM_CHANNEL,ui8DutyCycle);
	delay(1000);

	ui8DutyCycle = map(25,0,100,0,255);
	ledcWrite(GREEN_PWM_CHANNEL,ui8DutyCycle);
	ledcWrite(RED_PWM_CHANNEL,ui8DutyCycle);
	ledcWrite(BLUE_PWM_CHANNEL,ui8DutyCycle);
	delay(1000);

	ui8DutyCycle = map(50,0,100,0,255);
	ledcWrite(GREEN_PWM_CHANNEL,ui8DutyCycle);
	ledcWrite(RED_PWM_CHANNEL,ui8DutyCycle);
	ledcWrite(BLUE_PWM_CHANNEL,ui8DutyCycle);
	delay(1000);

	ui8DutyCycle = map(75,0,100,0,255);
	ledcWrite(GREEN_PWM_CHANNEL,ui8DutyCycle);
	ledcWrite(RED_PWM_CHANNEL,ui8DutyCycle);
	ledcWrite(BLUE_PWM_CHANNEL,ui8DutyCycle);
	delay(1000);

	ui8DutyCycle = map(100,0,100,0,255);
	ledcWrite(GREEN_PWM_CHANNEL,ui8DutyCycle);
	ledcWrite(RED_PWM_CHANNEL,ui8DutyCycle);
	ledcWrite(BLUE_PWM_CHANNEL,ui8DutyCycle);
	delay(1000);
}