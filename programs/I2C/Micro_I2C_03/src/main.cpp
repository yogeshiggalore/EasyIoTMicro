/*******************************************************************************
* Project Name: Micro_I2C_03
*
* Version: 1.0
*
* Description:
* This project, display real time clock value on oled
*
*
* Owner
* Yogesh M Iggalore
********************************************************************************
* Copyright (2021-22) EasyIoT
*******************************************************************************/
#include <Arduino.h>
#include <ESPRTC.h>

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
void display_rtc(void);
void blink_parameter(void);
void draw_text(byte x_pos, byte y_pos, char *text, byte text_size); 

char Time[]     = "  :  :  ";
char Calendar[] = "  /  /20  ";
char temperature[] = " 00.00";
char temperature_msb;
byte i, second, minute, hour, day, date, month, year, temperature_lsb;

uint16_t ui16Counter=0;
uint16_t ui16LastCounter=1;
uint8_t  ui8Second=0;

void setup() {
	delay(1000);

    /* start i2c and uart port */
	Wire.begin();
	Serial.begin(9600);
	
    Serial.println("\r\n");
	Serial.println("Micro_I2C_03");
	
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    	Serial.println(F("SSD1306 allocation failed"));
    	for(;;);
 	}

  	delay(2000);
	display.clearDisplay();
  	display.setTextColor(WHITE);
	/* start RTC */
	ERTC.Start();

    /* start timer module */
	noInterrupts();
    Timer_Start();
    interrupts();

	delay(1000);
	//ERTC.set_time(19,32,0,13,1,21);
	display_rtc();
	
}


void loop() {
	if(ui16LastCounter != ui16Counter){
        Serial.println(ERTC.Read());
		if(ui8Second != ERTC.ui8Second){
			display_rtc();
			ui8Second = ERTC.ui8Second;
		}
        ui16LastCounter = ui16Counter;
    }
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
    ui16Counter++;
	portEXIT_CRITICAL_ISR(&timerMux);
}
  
void display_rtc(void){
	display.clearDisplay();

	Time[7]     = ERTC.ui8Second % 10 + 48;
	Time[6]     = ERTC.ui8Second / 10 + 48;
	Time[4]     = ERTC.ui8Minute % 10 + 48;
	Time[3]     = ERTC.ui8Minute / 10 + 48;
	Time[1]     = ERTC.ui8Hour   % 10 + 48;
	Time[0]     = ERTC.ui8Hour   / 10 + 48;
	Calendar[9] = ERTC.ui8Year   % 10 + 48;
	Calendar[8] = ERTC.ui8Year   / 10 + 48;
	Calendar[4] = ERTC.ui8Month  % 10 + 48;
	Calendar[3] = ERTC.ui8Month  / 10 + 48;
	Calendar[1] = ERTC.ui8DayOfMonth   % 10 + 48;
	Calendar[0] = ERTC.ui8DayOfMonth   / 10 + 48;
	
	
	draw_text(4,  14, Calendar, 2);                     // Display the date (format: dd/mm/yyyy)
	draw_text(16, 35, Time, 2);                         // Display the time

	Serial.println(Calendar);
	Serial.println(Time);
}
 
void draw_text(byte x_pos, byte y_pos, char *text, byte text_size) {
	display.setCursor(x_pos, y_pos);
	display.setTextSize(text_size);
	display.print(text);
	display.display();
}