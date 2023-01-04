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

/* I2C display header */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* dht11 library */
#include <DHT.h>

/* define dht22 pins */
#define DHT11_PIN 33

/* define type as DHT11 */
#define DHTTYPE DHT11

/* create dht11 intence */
DHT dht(DHT11_PIN, DHTTYPE);

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
uint8_t read_dht11(void);

uint16_t ui16Counter=0;
uint16_t ui16LastCounter=1;

/* DHT11 veriables */
float fHumidity;
float fTemperature;
float fHeatIndex;

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

    /* start timer module */
	noInterrupts();
    Timer_Start();
    interrupts();

	/* start dht11 sensor */
	dht.begin();

}


void loop() {
	if(ui16LastCounter != ui16Counter){
		ui16LastCounter = ui16Counter;
        read_dht11();
		display.clearDisplay();
		display.setTextSize(3);
		display.setCursor(0,0);
		display.print(fTemperature);
		display.print("c");
		display.setCursor(0,30);
		display.setTextSize(3);
		display.print(fHumidity);
		display.print("%");
		display.display();
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
  
/* function to read dht11 sensor */
uint8_t read_dht11(void){

	uint8_t ui8Response = 0;

	/* read humidity */
	fHumidity = dht.readHumidity();

	/* read temperature */
	fTemperature = dht.readTemperature();

	/* check if data is valid */
	if (isnan(fHumidity) || isnan(fTemperature)){

		/* display read error in serial */
		fHeatIndex = 0;
		ui8Response = 0;
		Serial.println("DHT11 reading failure");
	}else{

		/* if data is valid read heat index */
		fHeatIndex = dht.computeHeatIndex(fHumidity, fTemperature, false);

		ui8Response = 1;

		/* display these values */
		Serial.printf("Temperature:%f degC Humidity:%f % Heat index:%f\n", fTemperature, fHumidity, fHeatIndex);
	}

	return ui8Response;
}