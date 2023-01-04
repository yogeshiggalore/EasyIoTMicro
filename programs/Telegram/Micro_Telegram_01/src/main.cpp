/*******************************************************************************
* Project Name: Micro_Telegram_01
*
* Version: 1.0
*
* Description:
* In this project esp32 uses telegram to control led pin
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* No Copyrights (2021-22) EasyIoT
*******************************************************************************/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

/* dht11 library */
#include <DHT.h>

/* Wifi credentials */
#define WIFI_SSID		"IOTDev"
#define WIFI_PASSWORD	"IOTDev1234"

/* bot name EasyIoTtestbot token */
#define BOTtoken "1107313877:AAHgo7IhhRbHzc8Fr6PmZ-zInm4hiLiff1U"

/* define dht11 pins */
#define DHT11_PIN 33

/* define type as DHT11 */
#define DHTTYPE DHT11

/* start secure wificlient*/
WiFiClientSecure client;

/* start telegram bot with token and wificlient */
UniversalTelegramBot bot(BOTtoken, client);

/*mean time between scan messages */
int iBotMTBS = 1000; 

/*last time messages' scan has been done */
long lBotLastTime;

#define GREEN_LED_PIN 2

uint8_t ui8LedStatus = 0;

/* function to handle telegram message */
void HandleNewMessages(int numNewMessages);

/* create dht22 intence */
DHT dht(DHT11_PIN, DHTTYPE);

/* DHT22 veriables */
float fHumidity;
float fTemperature;
float fHeatIndex;

/* function to read dht11 */
uint8_t read_dht11(void);

void setup(){
	/* serial with baudrate 9600 */
	Serial.begin(9600);

	//client.setInsecure();

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

	bot._debug = true;
	/* initialize digital GREEN_LED_PIN as an output. */
	pinMode(GREEN_LED_PIN, OUTPUT); 
	/* initialize pin as off */
	digitalWrite(GREEN_LED_PIN, LOW);

	/* start dht22 sensor */
	dht.begin();

	delay(1000);

	read_dht11();

}

void loop(void){
	if (millis() > lBotLastTime + iBotMTBS)  {
		int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
		while(numNewMessages) {
			Serial.println("got response");
			HandleNewMessages(numNewMessages);
			numNewMessages = bot.getUpdates(bot.last_message_received + 1);
		}
    	lBotLastTime = millis();
  	}
}

void HandleNewMessages(int numNewMessages) {
	Serial.println("handleNewMessages");
	Serial.println(String(numNewMessages));
	
	for (int i=0; i<numNewMessages; i++) {
		String chat_id = String(bot.messages[i].chat_id);
		String text = bot.messages[i].text;
		String from_name = bot.messages[i].from_name;
		Serial.print("Message received from ");
		Serial.println(from_name);
		if (from_name == "") from_name = "Guest";
			Serial.println(text.indexOf("on"));
			if (text.indexOf("on") > 0) {
				digitalWrite(GREEN_LED_PIN, HIGH);
				ui8LedStatus = 1;
				bot.sendMessage(chat_id, "Led is ON", "");
			}else if (text.indexOf("off") > 0) {
				ui8LedStatus = 0;
				digitalWrite(GREEN_LED_PIN, LOW);
				bot.sendMessage(chat_id, "Led is OFF", "");
			}else if (text.indexOf("status") > 0) {
				String sData;
				if(ui8LedStatus){
					sData = "Led is ON";
				}else {
					sData = "Led is OFF";
				}

				read_dht11();
				sData = "Led is OFF";
				sData = sData + "\nTemp: " + String(fTemperature) + ".c\nHumd: " + String(fHumidity) + "%";
				bot.sendMessage(chat_id, sData, "");
			}else if (text == "/start") {
				String keyboardJson = "[[\"/on\", \"/off\"],[\"/status\"]]";
				bot.sendMessageWithReplyKeyboard(chat_id, "Choose following options", "", keyboardJson, true);
			}else{
				bot.sendMessage(chat_id, "Wrong input type /start to get info ", "");
				String keyboardJson = "[[\"/on\", \"/off\"],[\"/status\"]]";
				bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
			}
  	}
}

/* function to read dht11 sensor */
uint8_t read_dht11(void)
{

	uint8_t ui8Response = 0;

	/* read humidity */
	fHumidity = dht.readHumidity();

	/* read temperature */
	fTemperature = dht.readTemperature();

	/* check if data is valid */
	if (isnan(fHumidity) || isnan(fTemperature))
	{

		/* display read error in serial */
		fHeatIndex = 0;
		ui8Response = 0;
		Serial.println("DHT11 reading failure");
	}
	else
	{

		/* if data is valid read heat index */
		fHeatIndex = dht.computeHeatIndex(fHumidity, fTemperature, false);

		ui8Response = 1;

		/* display these values */
		Serial.printf("Temperature:%f degC Humidity:%f % Heat index:%f\n", fTemperature, fHumidity, fHeatIndex);
	}

	return ui8Response;
}