
#include <Arduino.h>
#include <WiFi.h>
#include <LogLevel.h>
#include <logger.h>
#include <ModbusUtils.h>
#include <ModbusRTUMaster.h>
#include <ModbusRTUSlave.h>
#include <ModbusASCIIMaster.h>
#include <ModbusASCIISlave.h>
#include <ModbusTCPClient.h>
#include <ModbusTCPServer.h>
#include <ESPAsyncWebServer.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

LogLevel loglevel = LogLevel::Trace;

/* webserver object on port 80 */
AsyncWebServer server(80);

void setup() {
	delay(1000);
	Serial.begin(921600);
	Serial.println(" ");
	elog.start(LogLevel::Trace);
	
	elog.Write(LogLevel::Info,"setup", "Modbus demo");
	WiFi.disconnect();
	delay(100);
	WiFi.begin("IOTDev","IOTDev1234");
	elog.Write(LogLevel::Info,"setup", "Connecting to wifi");
	while(WiFi.status() != WL_CONNECTED){
		delay(500);
	}
	elog.Write(LogLevel::Info,"setup", "wifi connected");
	elog.Write(LogLevel::Info,"setup", WiFi.localIP().toString().c_str());
	ModUtil.start(MODBUS_MODE_RTU_SLAVE);
	ModUtil.test();

	//tcps.begin(WiFi.localIP(),502);
}

void loop() {
	//rtum.test();
	//rtum.update();
	//rtus.update();

	//ascim.test();
	//ascim.update();
	//ascis.update();

	//tcpc.update();
	//tcpc.test();
}