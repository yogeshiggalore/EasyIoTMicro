/*******************************************************************************
* Project Name: Micro_Internet_02
*
* Version: 1.0
*
* Description:
* In this project esp32 display weather station sOWMData on oled
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* No Copyrights (2020-21) EasyIoT
*******************************************************************************/

#include <ArduinoJson.h>
#include <WiFi.h>
#include <JsonListener.h>
#include <OpenWeatherMapCurrent.h>

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

/* initiate the client */
OpenWeatherMapCurrent OWMClient;

String OWM_APPID      = "90f11a74ecdcd36864145f01c773adf5";
String OWM_LOCATIONID = "1262321";
String OWM_LANGUAGE   = "en";

/* initiate the WifiClient */
WiFiClient wifiClient;

/* openweather sOWMData structure */
OpenWeatherMapCurrentData sOWMData;

/* open weather functions */
void Get_OpenWeather_Data(void);
void Display_OpenWeather_Data(void);

uint16_t ui16DisplayCounter;

void setup(){

	delay(2000);

	/* serial with baudrate 9600 */
	Serial.begin(9600);
	Serial.println("\n\n\n");
	
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

	/* start oled module */
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    	Serial.println(F("SSD1306 allocation failed"));
    	for(;;);
 	}
	delay(2000);
  	display.clearDisplay();
  	display.setTextColor(WHITE);

	Get_OpenWeather_Data();
	Display_OpenWeather_Data();

	/* start timer module */
	noInterrupts();
    Timer_1ms_Start();
    interrupts();
}

void loop(){
	/* display value every second */
	if(ui16DisplayCounter > 25000){
		ui16DisplayCounter = 0;
		Get_OpenWeather_Data();
		Display_OpenWeather_Data();
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

/* get open whether data */
void Get_OpenWeather_Data(void){

	OWMClient.setLanguage(OWM_LANGUAGE);
	OWMClient.setMetric(true);
	OWMClient.updateCurrentById(&sOWMData, OWM_APPID, OWM_LOCATIONID);
	
	/* "lon" and lat */
	Serial.printf("lon: %f lat: %f\n", sOWMData.lon,sOWMData.lat);
	
	/* weatherId */
	Serial.printf("weatherId: %d\n", sOWMData.weatherId);
	
	/* "main": "Rain", String main; */
	Serial.printf("main: %s\n", sOWMData.main.c_str());
	
	/* "description": "shower rain", String description; */
	Serial.printf("description: %s\n", sOWMData.description.c_str());
	
	/* "icon": "09d" String icon; String iconMeteoCon; */
	Serial.printf("icon: %s\n", sOWMData.icon.c_str());
	Serial.printf("iconMeteoCon: %s\n", sOWMData.iconMeteoCon.c_str());
	
	/* "temp": 290.56, float temp; */
	Serial.printf("temp: %f\n", sOWMData.temp);
	
	/* "pressure": 1013, uint16_t pressure; */
	Serial.printf("pressure: %d\n", sOWMData.pressure);
	
	/* "humidity": 87, uint8_t humidity; */
	Serial.printf("humidity: %d\n", sOWMData.humidity);
	
	/* "temp_min": 289.15, float tempMin; */
	Serial.printf("tempMin: %f\n", sOWMData.tempMin);
	
	/* "temp_max": 292.15 float tempMax; */
	Serial.printf("tempMax: %f\n", sOWMData.tempMax);
	
	/* "wind": {"speed": 1.5}, float windSpeed; */
	Serial.printf("windSpeed: %f\n", sOWMData.windSpeed);
	
	/* "wind": {"deg": 1.5}, float windDeg; */
	Serial.printf("windDeg: %f\n", sOWMData.windDeg);
	
	/* "clouds": {"all": 90}, uint8_t clouds; */
	Serial.printf("clouds: %d\n", sOWMData.clouds);
	
	/* "dt": 1527015000, uint64_t observationTime; */
	time_t time = sOWMData.observationTime;
	Serial.printf("observationTime: %d, full date: %s", sOWMData.observationTime, ctime(&time));
	
	/* "country": "CH", String country; */
	Serial.printf("country: %s\n", sOWMData.country.c_str());
	
	/* "sunrise": 1526960448, uint32_t sunrise; */
	time = sOWMData.sunrise;
	Serial.printf("sunrise: %d, full date: %s", sOWMData.sunrise, ctime(&time));
	
	/* "sunset": 1527015901 uint32_t sunset; */
	time = sOWMData.sunset;
	Serial.printf("sunset: %d, full date: %s", sOWMData.sunset, ctime(&time));
	
	/* "name": "Mysore", String cityName; */
	Serial.printf("cityName: %s\n", sOWMData.cityName.c_str());
	Serial.println();
}

/* display data on oled */
void Display_OpenWeather_Data(void){
	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(10,0);

	/* display set one */
	if(sOWMData.cityName.length() > 9){
		display.println(sOWMData.cityName.substring(0,9));
	}else{
		display.println(sOWMData.cityName.c_str());
	}
	
	display.setCursor(0,25);
	display.print("Lat:");
	display.print(sOWMData.lat);
	display.setCursor(0,50);
	display.print("Lon:");
	display.print(sOWMData.lon);
	display.display();

	delay(5000);
	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(10,0);
	display.println(sOWMData.main);
	display.setCursor(0,25);
	display.println(sOWMData.description);
	display.display();

	delay(5000);
	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(0,0);
	display.print("T:");
	display.println(sOWMData.temp);
	display.setCursor(0,25);
	display.print("P:");
	display.println(sOWMData.pressure);
	display.setCursor(0,50);
	display.print("H:");
	display.println(sOWMData.humidity);
	display.display();

	delay(5000);
	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(0,0);
	display.print("WSp:");
	display.println(sOWMData.windSpeed);
	display.setCursor(0,25);
	display.print("WDe:");
	display.println(sOWMData.windDeg);
	display.setCursor(0,50);
	display.print("Cloud:");
	display.println(sOWMData.clouds);
	display.display();
	
	delay(5000);
	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(0,0);
	display.print("   SUN");
	
	unsigned long hours = (sOWMData.sunrise % 86400L) / 3600;
    unsigned long minutes = (sOWMData.sunrise % 3600) / 60;
	minutes = minutes + 30;
	if(minutes > 60){
		minutes = minutes - 60;
		hours = hours + 1;
	}
	hours = hours + 5;
	String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);
    String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);
	display.setCursor(0,25);
	display.print("Rise ");
	display.print(hoursStr);
	display.print(":");
	display.println(minuteStr);

	display.setCursor(0,50);
	display.print("Set  ");
	hours = (sOWMData.sunset % 86400L) / 3600;
    minutes = (sOWMData.sunset % 3600) / 60;
	minutes = minutes + 30;
	if(minutes > 60){
		minutes = minutes - 60;
		hours = hours + 1;
	}
	hours = hours + 5;
	hoursStr = hours < 10 ? "0" + String(hours) : String(hours);
    minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);
	display.print(hoursStr);
	display.print(":");
	display.print(minuteStr);
	display.display();
	//delay(5000);

}