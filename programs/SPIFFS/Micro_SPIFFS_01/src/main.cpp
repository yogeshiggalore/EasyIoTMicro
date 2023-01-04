/*******************************************************************************
* Project Name:Micro_SPIFFS_01
*
* Version: 1.0
*
* Description:
* In this project esp32 handles file system using spiffs
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoT
*******************************************************************************/
#include <Arduino.h>
#include <SPIFFS.h>
 
void setup() {

	delay(1000);

	Serial.begin(9600);
	
	if (!SPIFFS.begin(true)) {
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}else{
		Serial.println("\r\nSPIFFS init");
	}
	
	File file = SPIFFS.open("/test.txt", FILE_WRITE);
	if (!file) {
		Serial.println("There was an error opening the file for writing");
		return;
  	}else{
		Serial.println("file open sucess ");
	}

  	if (file.print("TEST.txt")) {
    	Serial.println("File was written");
  	} else {
    	Serial.println("File write failed");
  	}
 
  	file.close();
}
 
void loop() {

}