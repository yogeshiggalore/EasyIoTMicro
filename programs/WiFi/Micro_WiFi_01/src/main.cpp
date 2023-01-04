/*******************************************************************************
* Project Name:Micro_WiFi_01
*
* Version: 1.0
*
* Description:
* In this project esp32 access point mode example
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoT
*******************************************************************************/
#include <Arduino.h>
#include <WiFi.h>

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

/* network credentials */
const char* ssid     = "ESP32-AP";
const char* password = "123456789";

void wifi_even_handler(WiFiEvent_t event);

void setup() {
	
	delay(2000);

	Serial.begin(9600);
	Wire.begin();

	/* Connect to Wi-Fi network with SSID and password */
  	Serial.println("Setting AP (Access Point)…");
  	
	/* wifi event handler */ 
	WiFi.onEvent(wifi_even_handler);

	/* Remove the password parameter, if you want the AP (Access Point) to be open */
  	WiFi.softAP(ssid, password);

	delay(50);

	/* wifi config details */
    WiFi.softAPConfig({192,168,4,2}, {192,168,4,1}, {255,255,255,0});

  	IPAddress IP = WiFi.softAPIP();
  	Serial.print("AP IP address: ");
  	Serial.println(IP);

	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    	Serial.println(F("SSD1306 allocation failed"));
    	for(;;);
 	}

  	display.clearDisplay();
  	display.setTextColor(WHITE);
	display.setTextSize(2);
	display.setCursor(0,0);
	display.println("AP mode");
	display.setCursor(0,20);
	display.setTextSize(1);
	display.print("ssid:");
	display.println(ssid);
	display.print("pass:");
	display.println(password);
	display.print("IP:");
	display.println(IP);
	display.print("GW:");
	display.println("192.168.4.1");
	display.display();
    delay(2000);
}

void loop(){

}

void wifi_even_handler(WiFiEvent_t event){
   switch (event){
		case SYSTEM_EVENT_STA_CONNECTED:
			Serial.println("Device WiFi got connected to external Router");
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			Serial.println("Device lost WiFi connection");
			WiFi.reconnect();
			break;
		case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
			Serial.println("Auth mode of exernal Router changed");
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
			Serial.print("Device WiFi got IP from external Router :");
			Serial.println(WiFi.localIP().toString());
			break;
		case SYSTEM_EVENT_AP_STACONNECTED:
			Serial.println("External device connected to this Access point");
			break;
		case SYSTEM_EVENT_AP_STADISCONNECTED:
			Serial.println("External device disconnected from this Access point");
			break;
		case SYSTEM_EVENT_SCAN_DONE:
			Serial.println("SYSTEM_EVENT_SCAN_DONE");
			break;
		case SYSTEM_EVENT_MAX:
			Serial.println("SYSTEM_EVENT_MAX");
			break;
		case SYSTEM_EVENT_WIFI_READY:
			Serial.println("SYSTEM_EVENT_WIFI_READY");
			break;           
		case SYSTEM_EVENT_STA_START:
			Serial.println("SYSTEM_EVENT_STA_START");
			break;
		case SYSTEM_EVENT_STA_STOP:            
			Serial.println("SYSTEM_EVENT_STA_STOP");
			break;
		case SYSTEM_EVENT_STA_LOST_IP:
			Serial.println("SYSTEM_EVENT_STA_LOST_IP");
			break;
		case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
			Serial.println("SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
			break;
		case SYSTEM_EVENT_STA_WPS_ER_FAILED:
			Serial.println("SYSTEM_EVENT_STA_WPS_ER_FAILED");
			break;
		case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
			Serial.println("SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
			break;
		case SYSTEM_EVENT_STA_WPS_ER_PIN:
			Serial.println("SYSTEM_EVENT_STA_WPS_ER_PIN");
			break;
		case SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP:
			Serial.println("SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP");
			break;
		case SYSTEM_EVENT_AP_START:
			Serial.println("SYSTEM_EVENT_AP_START");
			break;
		case SYSTEM_EVENT_AP_STOP:                
			Serial.println("SYSTEM_EVENT_AP_STOP");
			break;
		case SYSTEM_EVENT_AP_STAIPASSIGNED:       
			Serial.println("SYSTEM_EVENT_AP_STAIPASSIGNED");
			break;
		case SYSTEM_EVENT_AP_PROBEREQRECVED:      
			Serial.println("SYSTEM_EVENT_AP_PROBEREQRECVED");
			break;
		case SYSTEM_EVENT_GOT_IP6:                
			Serial.println("SYSTEM_EVENT_GOT_IP6");
			break;
		case SYSTEM_EVENT_ETH_START:              
			Serial.println("SYSTEM_EVENT_ETH_START");
			break;
		case SYSTEM_EVENT_ETH_STOP:               
			Serial.println("SYSTEM_EVENT_ETH_STOP");
			break;
		case SYSTEM_EVENT_ETH_CONNECTED:          
			Serial.println("SYSTEM_EVENT_ETH_CONNECTED");
			break;
		case SYSTEM_EVENT_ETH_DISCONNECTED:       
			Serial.println("SYSTEM_EVENT_ETH_DISCONNECTED");
			break;
		case SYSTEM_EVENT_ETH_GOT_IP:
			Serial.println("SYSTEM_EVENT_ETH_GOT_IP");
			break;
	}
}