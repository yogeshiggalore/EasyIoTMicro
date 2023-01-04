/*******************************************************************************
* Project Name: Micro_Internet_03
*
* Version: 1.0
*
* Description:
* In this project esp32 display world clock
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* No Copyrights (2021-22) EasyIoT
*******************************************************************************/
#include <ArduinoJson.h>
#include <WiFi.h>
#include <JsonListener.h>
#include <HTTPClient.h>
#include <WorldClockClient.h>

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

/* timer and intruppt function for 1 milisecond */
void Timer_1ms_Start(void);
void IRAM_ATTR timercallback1ms();

/* Wifi credentials */
#define WIFI_SSID		"IOTDev"
#define WIFI_PASSWORD	"IOTDev1234"

uint16_t ui16DisplayCounter;

String timeZoneIds [] = {"Asia/Kolkata","America/New_York", "Europe/London", "Europe/Paris" };
String sCityName[] = {"Kolkata","New_York","London","Paris"};
WorldClockClient worldClockClient("en", "IN", "E, dd. MMMMM yyyy", 4, timeZoneIds);

void Display_WorldClock(void);

void setup(){
	/* serial with baudrate 9600 */
	Serial.begin(9600);

	/* start oled module */
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    	Serial.println(F("SSD1306 allocation failed"));
    	for(;;);
 	}
	delay(2000);
  	display.clearDisplay();
  	display.setTextColor(WHITE);
	display.clearDisplay();
	display.setTextSize(3);
	display.setCursor(10,0);
	display.print("Wifi");
	display.display();

	/* Connect to Wi-Fi network with SSID and password */
	Serial.print("Connecting to ");
	Serial.println(WIFI_SSID);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.println(WiFi.status());
	}

  	/* Print local IP address */
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	display.clearDisplay();
	display.setTextSize(1);
	display.setCursor(0,0);
	display.print(WiFi.localIP());
	display.display();

  	/* start timer module */
	noInterrupts();
    Timer_1ms_Start();
    interrupts();
}

void loop(){
	/* display value every second */
	if(ui16DisplayCounter > 8000){
		ui16DisplayCounter = 0;
		worldClockClient.updateTime();
		Display_WorldClock();
	}
}

/* This function sets 1 milisecond timer */
void Timer_1ms_Start(void){

	/* set timer0 to 80MHz */
	timer = timerBegin(0, 80, true);

	/* attach interrupt to timer */
	timerAttachInterrupt(timer, &timercallback1ms, true);

	/* 1 ms interrupt */
	timerAlarmWrite(timer, 1000, true);

	/* enable timer */ 
	timerAlarmEnable(timer);
}

/* callback function 1ms interrupt */
void IRAM_ATTR timercallback1ms() {
	portENTER_CRITICAL_ISR(&timerMux);
	ui16DisplayCounter++;
	portEXIT_CRITICAL_ISR(&timerMux);
}

/* display data on oled */
void Display_WorldClock(void){
	String sTime;

	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(10,0);
	display.print(sCityName[0]);
	display.setTextSize(4);
	display.setCursor(0,25);
	sTime = worldClockClient.getHours(0) + ":" + worldClockClient.getMinutes(0);
	display.print(sTime);
	display.display();
	delay(2000);

	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(10,0);
	display.print(sCityName[1]);
	display.setTextSize(4);
	display.setCursor(0,25);
	sTime = worldClockClient.getHours(1) + ":" + worldClockClient.getMinutes(1);
	display.print(sTime);
	display.display();
	delay(2000);

	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(10,0);
	display.print(sCityName[2]);
	display.setTextSize(4);
	display.setCursor(0,25);
	sTime = worldClockClient.getHours(2) + ":" + worldClockClient.getMinutes(2);
	display.print(sTime);
	display.display();
	

	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(10,0);
	display.print(sCityName[3]);
	display.setTextSize(4);
	display.setCursor(0,25);
	sTime = worldClockClient.getHours(3) + ":" + worldClockClient.getMinutes(3);
	display.print(sTime);
	display.display();
	delay(2000);
}