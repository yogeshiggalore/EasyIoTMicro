/*******************************************************************************
* Project Name: EasyIoT_18_04
*
* Version: 1.0
*
* Description:
* In this project esp32 display astronomy
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* No Copyrights (2020-21) EasyIoT
*******************************************************************************/
#include <Arduino.h>
#include <WiFi.h>
#include <JsonListener.h>
#include <HTTPClient.h>
#include <Astronomy.h>

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

#define UTC_OFFSET 1
#define DST_OFFSET 1
#define NTP_SERVERS "in.pool.ntp.org", "pool.ntp.org"
#define EPOCH_1970

const String MOON_PHASES[] = {"New Moon", "Waxing Crescent", "First Quarter", "Waxing Gibbous",
                              "Full Moon", "Waning Gibbous", "Third quarter", "Waning Crescent"};

Astronomy astronomy;

uint16_t ui16DisplayCounter;

// initiate the WifiClient
WiFiClient wifiClient;

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

	configTime(UTC_OFFSET, DST_OFFSET * 60, NTP_SERVERS);
  Serial.println("Syncing time...");
  while(time(nullptr) <= 10000) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("Time sync'ed");

  // prepare the input values
  time_t now = time(nullptr);
  struct tm * timeinfo = localtime (&now);

  // now calculate the moon phase by the timestamp
  uint8_t phase = astronomy.calculateMoonPhase(now);
  Serial.printf("Moon Phase: %s\n", MOON_PHASES[phase].c_str());

  // another option is to use year, month and day (in month)
  phase = astronomy.calculateMoonPhase(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
  Serial.printf("Moon Phase: %s\n", MOON_PHASES[phase].c_str());

  // if you also need illumination there are two more methods which return a struct
  Astronomy::MoonData moonData = astronomy.calculateMoonData(now);
  Serial.printf("Moon Phase: %s\n", MOON_PHASES[moonData.phase].c_str());
  Serial.printf("Illumination: %f\n", moonData.illumination);

  moonData = astronomy.calculateMoonData(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
  Serial.printf("Moon Phase: %s\n", MOON_PHASES[moonData.phase].c_str());
  Serial.printf("Illumination: %f\n", moonData.illumination);
}

void loop(){
	/* display value every second */
	if(ui16DisplayCounter > 8000){
		ui16DisplayCounter = 0;
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