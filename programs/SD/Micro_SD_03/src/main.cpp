/*******************************************************************************
* Project Name:Micro_SD_03
*
* Version: 1.0
*
* Description:
* This project, logs temperature and humidity 
*
*
* Owner
* Yogesh M Iggalore
********************************************************************************
* Copyright (2021-22) EasyIoT
*******************************************************************************/

#include <Arduino.h>

#include <ESPRTC.h>

// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>

/* dht11 library */
#include <DHT.h>

#define SD_CS 5

/* define dht22 pins */
#define DHT11_PIN 33

/* define type as DHT11 */
#define DHTTYPE DHT11

/* create dht11 intence */
DHT dht(DHT11_PIN, DHTTYPE);

bool bSDStatus = false;
bool bFilePresent= false;

unsigned long ulTime=0;

/* DHT11 veriables */
float fHumidity;
float fTemperature;
float fHeatIndex;

String sData;

uint8_t read_dht11(void);

void setup() {
	delay(1000);

    Serial.begin(9600);
	Serial.println("\r\n");
	Serial.println("Micro_SD_02");
	
	/* Initialize SD card */   
	SPI.begin();
  	if(!SD.begin(SD_CS)) {
    	Serial.println("Card Mount Failed");
    	return;
  	}else{
		Serial.println("Card Mount sucess");
	}
  	
	/* check card type */
	uint8_t cardType = SD.cardType();
  	if(cardType == CARD_NONE) {
    	Serial.println("No SD card attached");
    	return;
  	}else{
		Serial.println("Initializing SD card...");
	}

  	if (!SD.begin(SD_CS)) {
    	Serial.println("ERROR - SD card initialization failed!");
    	return;    // init failed
  	}else{
		bSDStatus = true;  
		File file = SD.open("/logger.csv");
  		if(!file) {
    		Serial.println("File doens't exist");
  		}else {
    		Serial.println("File already exists");  
			bFilePresent = true;
  		}
  		file.close();
	}

	if(bFilePresent == false){
		File file = SD.open("/logger1.csv",FILE_WRITE);
    	file.println("Time,Temperature,Humidity,HeatIndex");
    	file.close();
	}

	ulTime = millis();

	/* start dht11 sensor */
	dht.begin();

	/* start rtc */
	ERTC.Start();

}

void loop() {

	if((millis() - ulTime) > 1000){
		ulTime = millis();
		if(bFilePresent){
			read_dht11();
			sData = ERTC.get_rtc_in_datetime_format() + "," + String(fTemperature) + "," + String(fHumidity) + "," + String(fHeatIndex);
			File file = SD.open("/logger1.csv",FILE_APPEND);
    		file.println(sData);
    		file.close();
			Serial.println(sData);
			Serial.println("data saved to sd card sucess ");
		}else{
			Serial.println("data saved to sd card failed ");
		}
	}
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