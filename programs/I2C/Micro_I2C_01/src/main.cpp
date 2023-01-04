/*******************************************************************************
* Project Name: Micro_I2C_01
*
* Version: 1.0
*
* Description:
* This project scans all i2c devices present in i2c network
*
*
* Owner
* Yogesh M Iggalore
********************************************************************************
* Copyright (2021-22) EasyIoT
*******************************************************************************/
#include <Arduino.h>
#include <Wire.h>


void setup() {
	delay(1000);

	/* start i2c and serial ports */
	Wire.begin();
	Serial.begin(9600);

	Serial.println(" Started project Micro_I2C_01");
}

void loop() {
	uint8_t ui8Error, ui8Address, ui8Devices;
	
	Serial.println("Scanning I2C port 0");
	ui8Devices = 0;
	
	/* scan i2c device from address 0 to 127 */
	for(ui8Address = 1; ui8Address < 127; ui8Address++ ){
    	/* 
			The i2c_scanner uses the return value of the Write.endTransmisstion to see if
    	 	a device did acknowledge to the ui8Address. 
		*/
		Wire.beginTransmission(ui8Address);
    	ui8Error = Wire.endTransmission();
 
    	if (ui8Error == 0){
      		Serial.print("I2C device found at address 0x");
      		if (ui8Address<16)
        		Serial.print("0");
      		Serial.print(ui8Address,HEX);
      		Serial.println("  !");
      		ui8Devices++;
    	}else if (ui8Error==4){
      		Serial.print("Unknown error at address 0x");
      		if (ui8Address<16)
        		Serial.print("0");
      		Serial.println(ui8Address,HEX);
    	} 
  	}

  	if(ui8Devices == 0){
		Serial.println("No I2C devices found\n");
	}else{
		Serial.println("done\n");
	}

	delay(15000);
}