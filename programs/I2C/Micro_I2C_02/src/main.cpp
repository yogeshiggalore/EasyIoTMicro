/*******************************************************************************
* Project Name: Micro_I2C_02
*
* Version: 1.0
*
* Description:
* This project, display count values in oled
*
*
* Owner
* Yogesh M Iggalore
********************************************************************************
* Copyright (2021-22) EasyIoT
*******************************************************************************/
#include <Arduino.h>
#include <Wire.h>

/* I2C display header */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* OLED display width, in pixels */
#define SCREEN_WIDTH 128 

/* OLED display height, in pixels */
#define SCREEN_HEIGHT 64 

/* Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/* hardware timer */
hw_timer_t * timer = NULL;

/* unlocking timer mux */
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

/* timer and intruppt function */
void Timer_Start(void);
void IRAM_ATTR timercallback();

void display_count_value(void);

uint16_t ui16Counter=0;
uint16_t ui16LastCounter=1;

void setup() {
    delay(1000);

    /* start i2c and uart port */
	Wire.begin();
	Serial.begin(9600);
	
    Serial.println("\r\n");
	Serial.println("Micro_I2C_02");
	
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    	Serial.println(F("SSD1306 allocation failed"));
    	for(;;);
 	}

  	delay(2000);
  	display.clearDisplay();
  	display.setTextColor(WHITE);
	display.setTextSize(4);
	display.setCursor(0,0);
	display.println("Hello");
	display.setCursor(0,30);
	display.setTextSize(4);
	display.println("Micro");
	display.display();
    delay(2000);

    /* start timer module */
	noInterrupts();
    Timer_Start();
    interrupts();

}

void loop() {
    if(ui16LastCounter != ui16Counter){
        display_count_value();
        ui16LastCounter = ui16Counter;
    }
}

/* This function sets timer */
void Timer_Start(void){

	/* set timer0 to 80MHz */
	timer = timerBegin(0, 80, true);

	/* attach interrupt to timer */
	timerAttachInterrupt(timer, &timercallback, true);

	/* 1000 ms interrupt */
	timerAlarmWrite(timer, 1000000, true);

	/* enable timer */ 
	timerAlarmEnable(timer);
}

/* interrupt callback function  */
void IRAM_ATTR timercallback() {
	portENTER_CRITICAL_ISR(&timerMux);
	
    ui16Counter++;
	portEXIT_CRITICAL_ISR(&timerMux);
}

void display_count_value(void){
    display.clearDisplay();
  	display.setTextColor(WHITE);
	display.setTextSize(4);
	display.setCursor(0,0);
	display.println("Count");
	display.setCursor(0,30);
	display.setTextSize(4);
	display.println(ui16Counter);
	display.display();
}