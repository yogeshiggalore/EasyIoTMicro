/*******************************************************************************
* Project Name:Micro_Internet_01
*
* Version: 1.0
*
* Description:
* In this project esp32 using ntp time 
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2021-22) , EasyIoT
*******************************************************************************/

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

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


const char* ssid     = "IOTDev";
const char* password = "IOTDev1234";

#define NTP_OFFSET  19800 // In seconds 
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "1.asia.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

void setup(){
	delay(1000);

    /* start i2c and uart port */
	Wire.begin();
	Serial.begin(9600);
	
    Serial.println("\r\n");
	Serial.println("Micro_Internet_01");
	
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    	Serial.println(F("SSD1306 allocation failed"));
    	for(;;);
 	}

	Serial.print("Connecting to wifi");
	Serial.println(ssid);
	display.clearDisplay();
  	display.setTextColor(WHITE);
	display.setTextSize(2);
	display.setCursor(0,0);
	display.println("Connecting to wifi");
	display.setCursor(0,30);
	display.println(ssid);
	display.display();
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	timeClient.begin();
}

void loop(){
	timeClient.update();
	String formattedTime = timeClient.getFormattedTime();

	Serial.println(formattedTime);

	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(0,0);
	display.println("NTP time");
	display.display();
	display.setTextSize(2);
	display.setCursor(0, 30);
	display.println(formattedTime);

	display.display();   // write the buffer to the display
	delay(5000);
}