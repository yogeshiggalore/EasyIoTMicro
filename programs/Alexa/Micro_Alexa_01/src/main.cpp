/*******************************************************************************
* Project Name: Micro_Alexa_01
*
* Version: 1.0
*
* Description:
* In this project esp32 handles alexa connections. here sinric helps to
* integrate esp32 hardware to alexa skills
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

/* Device ID in sinric.com */
#define DEVICEID "5ed50c24d8c9f16bc25d2dc2"

#define DEVICE_PIN  25

#define ON  1
#define OFF 0

/* websocket and wificlient instance */
WebSocketsClient webSocket;
WiFiClient client;

/* webscoket event handler */
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

uint64_t ui64HeartbeatTimestamp = 0;
bool bIsConnected = false;

void setup(){
    Serial.begin(9600);
    Serial.println("\n\nAlexa_Demo");
    pinMode(DEVICE_PIN,OUTPUT);

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
            DynamicJsonDocument json(1024);
            deserializeJson(json, (char*) payload);
            String deviceId = json ["deviceId"];     
            String action = json ["action"];
            
            if(deviceId == DEVICEID){
                /* Alexa command */
                if(action == "setPowerState") {
                    String value = json ["value"];
                    if(value == "ON") {
                        digitalWrite(DEVICE_PIN,ON);
                    } else {
                        digitalWrite(DEVICE_PIN,OFF);
                    }
                }else if (action == "test") {
                    Serial.println("[WSc] received test command from sinric.com");
                }/* google command */
                else if(action == "action.devices.commands.OnOff"){
                    String value = json ["value"]["on"];
                    Serial.println(value);
                    if(value == "true") {
                        digitalWrite(DEVICE_PIN,ON);
                    } else {
                        digitalWrite(DEVICE_PIN,OFF);
                    }
                }
            }else{
                Serial.println("device id mismatch");
            }
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
