/*******************************************************************************
* Project Name: Micro_Alexa_02
*
* Version: 1.0
*
* Description:
* In this project esp32 handles alexa connections. here sinric helps to
* control rgb led
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* No Copyrights (2020-21) EasyIoT
*******************************************************************************/

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <StreamString.h>

/* heartbeat interval between websocket client and server */
#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

/* API key of siniric account */
#define APIKEY "6cbd92b8-caf8-45f7-b749-0297e971fc1e"

/* Wifi SSID and password */
#define SSID     "IOTDev"
#define PASSWORD "IOTDev1234"

#define DEVICE_ON   "ON"
#define DEVICE_OFF   "OFF"

/* Device ID in sinric.com */
#define DEVICEID "5cf259a7bfe01b2db3b7197f"

/* define led pin */
#define  GREEN_LED_PIN	2
#define  RED_LED_PIN	4
#define  BLUE_LED_PIN	13

/* set pwm frequency,channel and resolution */
#define PWM_FREQUENCY	5000

#define GREEN_PWM_CHANNEL	0
#define RED_PWM_CHANNEL		1
#define BLUE_PWM_CHANNEL	2

#define PWM_RESOLUTION	8

/* websocket and wificlient instance */
WebSocketsClient webSocket;
WiFiClient client;

/* webscoket event handler */
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
uint8_t clamp( uint8_t x, uint8_t min, uint8_t max);
void color_temperature_to_rgb(uint16_t ui16Value,uint8_t color[3]);
void hsb_to_rgb(int H, double S, double V, uint8_t color[3]);
void check_for_action(String sAction, String sValue,int i16jsonValue, int* ai16ResponseData,uint8_t color[3]);
uint8_t check_for_device_id(String sDeviceID);
void handle_alexa_request(uint8_t * payload);
void set_led_color(uint8_t ui8Red, uint8_t ui8Green, uint8_t ui8Blue);

uint64_t ui64HeartbeatTimestamp = 0;
bool bIsConnected = false;
uint8_t ui8ValueRed;
uint8_t ui8ValueGreen;
uint8_t ui8ValueBlue;

void setup(){
    Serial.begin(9600);
    Serial.println("\n\nAlexa_Demo");

    /* configure PWM functionalities */
	ledcSetup(GREEN_PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION);
	ledcSetup(RED_PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION);
	ledcSetup(BLUE_PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION);

	/* attach led pin to PWM */
	ledcAttachPin(GREEN_LED_PIN,GREEN_PWM_CHANNEL);
	ledcAttachPin(RED_LED_PIN,RED_PWM_CHANNEL);
	ledcAttachPin(BLUE_LED_PIN,BLUE_PWM_CHANNEL);

	ledcWrite(GREEN_PWM_CHANNEL,0);
	ledcWrite(RED_PWM_CHANNEL,0);
	ledcWrite(BLUE_PWM_CHANNEL,0);

    WiFi.begin(SSID, PASSWORD);
    Serial.println("Connecting to wifi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    if(WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.print("WiFi connected. ");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }

    /* server address, port and URL */
    webSocket.begin("iot.sinric.com", 80, "/");
    /* event handler */
    webSocket.onEvent(webSocketEvent);
    /* authorization to server with apikey */
    webSocket.setAuthorization("apikey", APIKEY);
    /* try again every 5000ms if connection has failed */
    webSocket.setReconnectInterval(5000);
}

void loop(){
    webSocket.loop();
    if(bIsConnected) {
        uint64_t now = millis();
        if((now - ui64HeartbeatTimestamp) > HEARTBEAT_INTERVAL) {
            ui64HeartbeatTimestamp = now;
            webSocket.sendTXT("H");
        }
    }
}

/****************************************************************************** 
* Function Name: webSocketEvent
*******************************************************************************
*
* Summary:
*  This function calls when web socket happens 
*
* Parameters:  
*  WStype_t type, payload, length
*
* Return: 
*  None
*  
*******************************************************************************/
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            bIsConnected = false;
            Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
        break;
        case WStype_CONNECTED: {
            bIsConnected = true;
            Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
            Serial.printf("Waiting for commands from sinric.com ...\n");
        }
        break;
        case WStype_TEXT: {
            Serial.printf("[WSc] get text: %s\n", payload);
            handle_alexa_request(payload);
        }
        break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);
        break;
        default:
            Serial.print("default websocket happens code: ");
            Serial.println(type);
            break;
    }
}

void handle_alexa_request(uint8_t * payload){
    DynamicJsonDocument json(1024);
    deserializeJson(json, payload,DeserializationOption::NestingLimit(20));
    uint8_t ui8DeviceIndex=0;
    uint8_t aui8RGB[3];
    uint16_t ui16Hue;
    double dSaturation;
    double dBrightness;

    String sDeviceId = json ["deviceId"];
    String sAction = json ["action"];
    String sValue = json["value"];
    
    Serial.println(sDeviceId);
    Serial.println(sAction);
    Serial.println(sValue);

    int i16jsonValue=0;
    int ai16ResponseData[3];

    ai16ResponseData[0] = 0x7FFF;
    ai16ResponseData[1] = 0x7FFF;
    ai16ResponseData[2] = 1;

    if(sAction == "SetPercentage"){
        i16jsonValue = json["value"]["percentage"];
    }else if(sAction == "AdjustPercentage"){
        i16jsonValue = json["value"]["percentageDelta"];
    }else if(sAction == "action.devices.commands.OnOff"){
        String sString1 = json["value"]["on"];
        Serial.println(sString1);
        if(sString1 == "true"){
            i16jsonValue = 1;
            Serial.println("OK");
        }else{
            i16jsonValue = 0;
            Serial.println("Error");
        }
    }else if(sAction == "action.devices.commands.BrightnessAbsolute"){
        String sString1 = json["value"]["brightness"];
        Serial.println(sString1);
        i16jsonValue = json["value"]["brightness"];
        Serial.println("brigthness");
        Serial.println(i16jsonValue);
    }else if(sAction == "SetColor"){
        ui16Hue = json["value"]["hue"];
        dSaturation = json["value"]["saturation"];
        dBrightness  = json["value"]["brightness"];
        hsb_to_rgb(ui16Hue,dSaturation,dBrightness,aui8RGB);
        
    }else if(sAction == "SetColorTemperature"){
        Serial.print("color temp value ");
        Serial.println(sValue);
        color_temperature_to_rgb(sValue.toInt(),aui8RGB);
    }else{
        i16jsonValue =  0;
    }

    Serial.print("Sinric data: ");
    Serial.print(sDeviceId);
    Serial.print(" ");
    Serial.print(sAction);
    Serial.print(" ");
    Serial.print(sValue);
    Serial.print(" ");
    Serial.println(i16jsonValue);

    //serializeJsonPretty(json,sString);
    ui8DeviceIndex = check_for_device_id(sDeviceId);

    if(ui8DeviceIndex != 0xFF){
        check_for_action(sAction,sValue,i16jsonValue,ai16ResponseData,aui8RGB);
    }else{
        Serial.print("No config id:");
        Serial.println(sDeviceId);
    }
}

uint8_t check_for_device_id(String sDeviceID){
    uint8_t ui8Index=0xFF;
 
    if(sDeviceID.equals(DEVICEID)){
        //Serial.print("Device is correct:");
        //Serial.println(MAlexa.sAlexaACInput1ID);
        Serial.println(DEVICEID);
        ui8Index =  1;
    }else{
        ui8Index=0xFF;
    }

    return ui8Index;
}


void check_for_action(String sAction, String sValue,int i16jsonValue, int* ai16ResponseData,uint8_t color[3]){
    int i16Value;
    int ui16Bright;
     
    if(sAction.equals("setPowerState")){
        
        if(sValue.equals(DEVICE_ON)){
            set_led_color(ui8ValueRed,ui8ValueGreen,ui8ValueBlue);
        }else{
            ledcWrite(GREEN_PWM_CHANNEL,0);
	        ledcWrite(RED_PWM_CHANNEL,0);
	        ledcWrite(BLUE_PWM_CHANNEL,0);	
        }
    }else if(sAction.equals("SetBrightness")){
        i16Value = sValue.toInt();
        if(i16Value > 100){
            i16Value = 100;
        }
        if(i16Value < 0){
            i16Value = 0;
        }

        //Pixel.setBrightness(map(i16Value,0,100,0,255));

    }else if(sAction.equals("SetPercentage")){
        //Pixel.setBrightness(map(i16jsonValue,0,100,0,255));
    }else if(sAction.equals("AdjustBrightness")){
        if(sValue.toInt() < 0){
            ui16Bright = sValue.toInt() * -1;
            ui16Bright = map(ui16Bright,0,100,0,255);
            //if(Pixel.ui8BrightValue > ui16Bright){
            //    Pixel.ui8BrightValue = Pixel.ui8BrightValue - ui16Bright;
            //}
        }else{
            ui16Bright = map(sValue.toInt(),0,100,0,255);
            //if((Pixel.ui8BrightValue + ui16Bright) < 255){
            //    Pixel.ui8BrightValue = Pixel.ui8BrightValue + ui16Bright;
            //}
        }
        //Pixel.setBrightness(Pixel.ui8BrightValue);

    }else if(sAction.equals("AdjustPercentage")){
        if(i16jsonValue < 0){
            ui16Bright = i16jsonValue * -1;
            ui16Bright = map(ui16Bright,0,100,0,255);
            //if(Pixel.ui8BrightValue > ui16Bright){
            //    Pixel.ui8BrightValue = Pixel.ui8BrightValue - ui16Bright;
            //}
        }else{
            ui16Bright = map(i16jsonValue,0,100,0,255);
            //if((Pixel.ui8BrightValue + ui16Bright) < 255){
            //    Pixel.ui8BrightValue = Pixel.ui8BrightValue + ui16Bright;
            //}
        }
        //Pixel.setBrightness(Pixel.ui8BrightValue);
    }else if(sAction == "action.devices.commands.OnOff"){
        if(i16jsonValue){
            //Pixel.setBrightness(255);
            //ui8AlexaUpdated = 1;
			set_led_color(ui8ValueRed,ui8ValueGreen,ui8ValueBlue);
        }else{
            //Pixel.setBrightness(0);
            //ui8AlexaUpdated = 1;
			ledcWrite(GREEN_PWM_CHANNEL,0);
	        ledcWrite(RED_PWM_CHANNEL,0);
	        ledcWrite(BLUE_PWM_CHANNEL,0);	
        }
    }else if(sAction == "SetColor"){
        //Pixel.Set_LightOnColor(color[0],color[1],color[2]);
		set_led_color(color[0],color[1],color[2]);

    }else if(sAction == "action.devices.commands.BrightnessAbsolute"){
        if(i16jsonValue > 100){
            i16jsonValue = 100;
        }
        if(i16jsonValue < 0){
            i16jsonValue = 0;
        }
        //Pixel.setBrightness(map(i16jsonValue,0,100,0,255)); ;
    }else if(sAction == "SetColorTemperature"){
        //Pixel.Set_LightOnColor(color[0],color[1],color[2]);
		set_led_color(color[0],color[1],color[2]);
    }else{
        // do nothing
        ai16ResponseData[2] = 0;
    }
}

void hsb_to_rgb(int H, double S, double V, uint8_t color[3]) {
	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = V - C;
	double Rs, Gs, Bs;

	if(H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;	
	}
	else if(H >= 60 && H < 120) {	
		Rs = X;
		Gs = C;
		Bs = 0;	
	}
	else if(H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;	
	}
	else if(H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;	
	}
	else if(H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;	
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;	
	}
	
	color[0] = (Rs + m) * 255;
	color[1] = (Gs + m) * 255;
	color[2] = (Bs + m) * 255;
}


void color_temperature_to_rgb(uint16_t ui16Value,uint8_t color[3]){

    uint16_t temp = ui16Value / 100;
    uint16_t r,g,b;

    //var red, green, blue;

    if( temp <= 66 ){ 

        r = 255; 
        
        g = temp;
        g = 99.4708025861 * log(g) - 161.1195681661;

        if( temp <= 19){

            b = 0;

        } else {

            b= temp-10;
            b = 138.5177312231 * log(b) - 305.0447927307;

        }

    } else {

        r = temp - 60;
        r = 329.698727446 * pow(r, -0.1332047592);
        
        g = temp - 60;
        g = 288.1221695283 * pow(g, -0.0755148492 );

        b = 255;

    }

    color[0] = clamp(r,0, 255);
    color[1] = clamp(g,0, 255);
    color[2] = clamp(b,0, 255);
}


uint8_t clamp( uint8_t x, uint8_t min, uint8_t max) {
    uint8_t ui8FinalValue;

    if(x<min){
        ui8FinalValue = min; 
    }else if(x>max){ 
        ui8FinalValue = max; 
    }else{
        ui8FinalValue = x;
    }

    return ui8FinalValue;
}

void set_led_color(uint8_t ui8Red, uint8_t ui8Green, uint8_t ui8Blue){
	
    ui8ValueRed  = ui8Red;
	ui8ValueGreen = ui8Green;
	ui8ValueBlue = ui8Blue; 

    ledcWrite(GREEN_PWM_CHANNEL,ui8ValueGreen);
	ledcWrite(RED_PWM_CHANNEL,ui8ValueRed);
	ledcWrite(BLUE_PWM_CHANNEL,ui8ValueBlue);	
}