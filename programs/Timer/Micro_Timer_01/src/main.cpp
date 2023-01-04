/*******************************************************************************
* Project Name: Micro_Timer_01
*
* Version: 1.0
*
* Description:
* In this project esp32, switches led on/off using timer.
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2021-22) , EasyIoTMicro
*******************************************************************************/

/* include header files */
#include <Arduino.h>

/* set led pin */
#define GREEN_LED_PIN 2

/* led on off */
#define LED_OFF	0
#define LED_ON	1

uint8_t ui8LEDState = 0;

/* hardware timer */
hw_timer_t * timer = NULL;

/* unlocking timer mux */
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

/* timer and intruppt function */
void Timer_Start(void);
void IRAM_ATTR timercallback();

void setup() {

	/* set LED pin to output*/
	pinMode(GREEN_LED_PIN, OUTPUT);

	/* set led initaial state to LOW */
	digitalWrite(GREEN_LED_PIN,LOW);
	ui8LEDState = LED_OFF;

	/* start timer module */
	noInterrupts();
    Timer_Start();
    interrupts();
}

void loop() {

}

/* This function sets timer */
void Timer_Start(void){

	/* set timer0 to 80MHz */
	timer = timerBegin(0, 80, true);

	/* attach interrupt to timer */
	timerAttachInterrupt(timer, &timercallback, true);

	/* 500 ms interrupt */
	timerAlarmWrite(timer, 500000, true);

	/* enable timer */ 
	timerAlarmEnable(timer);
}

/* interrupt callback function  */
void IRAM_ATTR timercallback() {
	portENTER_CRITICAL_ISR(&timerMux);
	if(ui8LEDState == LED_ON){
		digitalWrite(GREEN_LED_PIN,LOW);
		ui8LEDState = LED_OFF;
	}else{
		digitalWrite(GREEN_LED_PIN,HIGH);
		ui8LEDState = LED_ON;
	}
	portEXIT_CRITICAL_ISR(&timerMux);
}