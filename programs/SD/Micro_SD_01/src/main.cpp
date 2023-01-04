/*******************************************************************************
* Project Name:Micro_SD_01
*
* Version: 1.0
*
* Description:
* This project test sd card modules
*
*
* Owner
* Yogesh M Iggalore
********************************************************************************
* Copyright (2021-22) EasyIoT
*******************************************************************************/

#include <Arduino.h>

// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>

#define SD_CS 5

void setup() {
	delay(1000);

    Serial.begin(9600);
	Serial.println("\r\n");
	Serial.println("Micro_SD_01");
	
	/* Initialize SD card */   
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
		File file = SD.open("/test1.txt");
  		if(!file) {
    		Serial.println("File doens't exist");
  		}else {
    		Serial.println("File already exists");  
  		}
  		file.close();
	}
}

void loop() {

}