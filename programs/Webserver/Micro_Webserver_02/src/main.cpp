/*******************************************************************************
* Project Name:Micro_Webserver_02
*
* Version: 1.0
*
* Description:
* In this project esp32 uses station mode to display temperature and humidity
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

/* dht11 library */
#include <DHT.h>


/* Wifi credentials */
#define WIFI_SSID		"IOTDev"
#define WIFI_PASSWORD	"IOTDev1234"

/* define dht22 pins */
#define DHT11_PIN 33

/* define type as DHT11 */
#define DHTTYPE DHT11

/* create dht11 intence */
DHT dht(DHT11_PIN, DHTTYPE);

/* webserver object on port 80 */
WebServer ESPServer(80);

/*web page index */
const char* htmlfile = "/index.html";

/* ESP Server functions*/
void espserver_handle_webrequests(void);
void espserver_dht_update(void);
void espserver_handle_root(void);
bool espserver_load_from_spiffs(String path);

/* hardware timer */
hw_timer_t * timer = NULL;

/* unlocking timer mux */
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

/* timer and intruppt function for 1000 milisecond */
void timer_1000ms_start(void);
void IRAM_ATTR timercallback1000ms();
uint8_t read_dht11(void);

bool bReadFlag=0;
float fTemperature;
float fHumidity;
float fHeatIndex;

void setup() {
	/* set UART baudrate to 9600 */
	Serial.begin(9600);
		
	/* Connect to Wi-Fi network with SSID and password */
	Serial.print("Connecting to ");
	Serial.println(WIFI_SSID);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.println(WiFi.status());
	}
	
	/* Print local IP address and start web server */
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	
	/* Initialize SPIFFS */
	if(!SPIFFS.begin()){
		Serial.println("An Error has occurred while mounting SPIFFS");
		while(1);
	}

	/* Enable ESP Server function and start webserver */
    ESPServer.on("/",espserver_handle_root);
    ESPServer.on("/dhtupdate",espserver_dht_update);
    ESPServer.onNotFound(espserver_handle_webrequests);
    ESPServer.begin();

	/* start timer module */
	noInterrupts();
    timer_1000ms_start();
    interrupts();

	dht.begin(DHT11_PIN);
}	

void loop(){
	/* function to handle clients */
	ESPServer.handleClient();

	if(bReadFlag){
		bReadFlag = 0;
		read_dht11();
	}
}


/* This function sets 1000 milisecond timer */
void timer_1000ms_start(void){

	/* set timer0 to 80MHz */
	timer = timerBegin(0, 80, true);

	/* attach interrupt to timer */
	timerAttachInterrupt(timer, &timercallback1000ms, true);

	/* 1000 ms interrupt */
	timerAlarmWrite(timer, 1000000, true);

	/* enable timer */ 
	timerAlarmEnable(timer);
}

/* callback function 1ms interrupt */
void IRAM_ATTR timercallback1000ms() {
	portENTER_CRITICAL_ISR(&timerMux);

	bReadFlag = 1;
 	portEXIT_CRITICAL_ISR(&timerMux);
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

/* this function updates the dht values on client request */
void espserver_dht_update(void){
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
    
	doc["temp"] = String(fTemperature); 

	doc["humd"] = String(fHumidity);

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