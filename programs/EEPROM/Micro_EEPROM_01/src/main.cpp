/*******************************************************************************
* Project Name:Micro_EEPROM_01
*
* Version: 1.0
*
* Description:
* In this project esp32 handles web pages using spiffs and controls LED and stores LED 
* status in EEPROM
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoT
*******************************************************************************/

/* include headers */
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#define SSID		"IOTDev"
#define PASSWORD 	"IOTDev1234"

#define MEMORY_SIZE       10

#define MEMORY_GREEN_LED_BYTE   0
#define MEMORY_INIT_BYTE        9

#define GREEN_LED_PIN	2

/* web page index */
const char* htmlfile = "/index.html";

/* webserver object on port 80 */
WebServer ESPServer(80);

uint8_t ui8MemoryGreenLEDValue=0;

/* ESP Server functions*/
void espserver_handle_webrequests(void);
void espserver_led_update(void);
void espserver_handle_root(void);
bool espserver_load_from_spiffs(String path);

/* Memory function */
void eeprom_init(void);
void eeprom_update_all(void);

void setup(){
	
    delay(1000);

    /* start serial port with speed 9600 */
    Serial.begin(9600);
	Serial.setDebugOutput(true);

    /* start EEPROM */
    EEPROM.begin(MEMORY_SIZE);
    eeprom_init();

	/* wifi begin */
	WiFi.begin(SSID, PASSWORD);

	/* wait for wifi to connect */
	Serial.println("Connecting to WiFi..");
	while (WiFi.status() != WL_CONNECTED) {
  		delay(500);
  		Serial.println(WiFi.status());
	}

	/* Initialize SPIFFS */
	if(!SPIFFS.begin()){
		Serial.println("An Error has occurred while mounting SPIFFS");
		while(1);
	}

	/* Enable ESP Server function and start webserver */
    ESPServer.on("/",espserver_handle_root);
    ESPServer.on("/ledupdate",espserver_led_update);
    ESPServer.onNotFound(espserver_handle_webrequests);
    ESPServer.begin();

    /* set led pin as output and set to low */
	pinMode(GREEN_LED_PIN,OUTPUT);
	/* update led memory status to led */
    if(ui8MemoryGreenLEDValue == HIGH){
        digitalWrite(GREEN_LED_PIN,HIGH);
    }else{
        digitalWrite(GREEN_LED_PIN,LOW);
    }
}

void loop(){
    /* function to handle clients */
	ESPServer.handleClient();
}

/* this function used to load all web server related files from spiffs */
bool espserver_load_from_spiffs(String path){
    String dataType = "text/plain";
    if(path.endsWith("/")) path += "index.htm";
    if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
    else if(path.endsWith(".html")) dataType = "text/html";
    else if(path.endsWith(".htm")) dataType = "text/html";
    else if(path.endsWith(".css")) dataType = "text/css";
    else if(path.endsWith(".js")) dataType = "application/javascript";
    else if(path.endsWith(".png")) dataType = "image/png";
    else if(path.endsWith(".gif")) dataType = "image/gif";
    else if(path.endsWith(".jpg")) dataType = "image/jpeg";
    else if(path.endsWith(".ico")) dataType = "image/x-icon";
    else if(path.endsWith(".xml")) dataType = "text/xml";
    else if(path.endsWith(".pdf")) dataType = "application/pdf";
    else if(path.endsWith(".zip")) dataType = "application/zip";
    File dataFile = SPIFFS.open(path.c_str(), "r");
    if (ESPServer.hasArg("download")) dataType = "application/octet-stream";
    if (ESPServer.streamFile(dataFile, dataType) != dataFile.size()) {}

    dataFile.close();
    return true;
}

/* this function updates the led status on client request */
void espserver_led_update(void){
   	String sData;
    StaticJsonDocument<1048> doc;

    /* Measure Signal Strength (RSSI) of Wi-Fi connection */
    if(WiFi.RSSI() <= -100){
        doc["rssi"] = 0;
    }else if(WiFi.RSSI() >= -50){
        doc["rssi"] = 100;
    }else{
        doc["rssi"] = 2 * (WiFi.RSSI() + 100);
    }

    doc["MAC"] = WiFi.macAddress();
    
    Serial.println(ESPServer.arg("state"));
	if (ESPServer.arg("state")=="0"){
		digitalWrite(GREEN_LED_PIN,LOW);
        EEPROM.write(MEMORY_GREEN_LED_BYTE,0);
        EEPROM.commit();
		Serial.println("OFF");
        doc["LEDState"] = "OFF";
	}else{
		digitalWrite(GREEN_LED_PIN,HIGH);
        EEPROM.write(MEMORY_GREEN_LED_BYTE,1);
        EEPROM.commit();
		Serial.println("ON");
        doc["LEDState"] = "ON";
	}

    serializeJson(doc, sData);

    ESPServer.send(200, "text/json",sData);
}

/* This handling root file */
void espserver_handle_root(void){
  ESPServer.sendHeader("Location", "/index.html",true);   //Redirect to our html web page
  ESPServer.send(302, "text/plane","");
}

/* this function handle when wrong url path requented */
void espserver_handle_webrequests(void){
    if(espserver_load_from_spiffs(ESPServer.uri())) return;
    String message = "File Not Detected\n\n";
    message += "URI: ";
    message += ESPServer.uri();
    message += "\nMethod: ";
    message += (ESPServer.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += ESPServer.args();
    message += "\n";
    for (uint8_t i=0; i<ESPServer.args(); i++){
        message += " NAME:"+ESPServer.argName(i) + "\n VALUE:" + ESPServer.arg(i) + "\n";
    }

    ESPServer.send(404, "text/plain", message);
}

/* ini memory and update all bytes*/
void eeprom_init(void){
    if(EEPROM.read(MEMORY_INIT_BYTE) != 1){
        EEPROM.write(MEMORY_GREEN_LED_BYTE,0);
        EEPROM.write(MEMORY_INIT_BYTE,1);
        EEPROM.commit();
    }

	/* update all memory bytes */
	eeprom_update_all();
}

/* This function updates all memory bytes */
void eeprom_update_all(void){
	ui8MemoryGreenLEDValue = EEPROM.read(MEMORY_GREEN_LED_BYTE);
}
