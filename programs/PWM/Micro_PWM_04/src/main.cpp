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

void rgb_color(uint8_t ui8Red, uint8_t ui8Green, uint8_t ui8Blue);

void setup(){

	/* configure PWM functionalities */
	ledcSetup(GREEN_PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION);
	ledcSetup(RED_PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION);
	ledcSetup(BLUE_PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION);

	/* attach led pin to PWM */
	ledcAttachPin(GREEN_LED_PIN,GREEN_PWM_CHANNEL);
	ledcAttachPin(RED_LED_PIN,RED_PWM_CHANNEL);
	ledcAttachPin(BLUE_LED_PIN,BLUE_PWM_CHANNEL);
	pinMode(25,OUTPUT);
	
}

void loop(){
	digitalWrite(25,HIGH);
	rgb_color(255, 0, 0); // Red
  	delay(1000);
  	rgb_color(0, 255, 0); // Green
  	delay(1000);
 	rgb_color(0, 0, 255); // Blue
  	delay(1000);
  	rgb_color(255, 255, 125); // Raspberry
  	delay(1000);
  	rgb_color(0, 255, 255); // Cyan
  	delay(1000);
  	rgb_color(255, 0, 255); // Magenta
  	delay(1000);
  	rgb_color(255, 255, 0); // Yellow
  	delay(1000);
  	rgb_color(255, 255, 255); // White
  	delay(1000);
	digitalWrite(25,LOW);
}

void rgb_color(uint8_t ui8Red, uint8_t ui8Green, uint8_t ui8Blue){
	ledcWrite(GREEN_PWM_CHANNEL,ui8Green);
	ledcWrite(RED_PWM_CHANNEL,ui8Red);
	ledcWrite(BLUE_PWM_CHANNEL,ui8Blue);
}