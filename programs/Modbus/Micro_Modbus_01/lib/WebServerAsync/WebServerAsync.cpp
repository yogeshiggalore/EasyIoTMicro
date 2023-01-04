/*#include <Arduino.h>
#include <WebServerAsync.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
//#include <main.h>

void setup_server(void){
	// Enable ESP Server function and start webserver 
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		server_handle_root(request);
		// server_update_Wifi_config(request);
	});

	server.on("/updateMqttConfig", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_update_mqtt_config(request);
	});

	server.on("/updateWifiConfig", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_update_Wifi_config(request);
	});

	server.on("/getDeviceConfig", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_get_device_config(request);
	});

	server.on("/getSensorValues", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_get_sensor_values(request);
	});

	server.on("/resetDevice", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_reset_device(request);
	});

	server.on("/getWifiDevices", HTTP_POST, [](AsyncWebServerRequest *request) {
		meti_wifi_scan_networks(request);
	});

	server.on("/getAllModbusDrivers", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_get_all_modbus_driver(request);
	});

	server.on("/saveNewModbusDriver", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_save_new_modbus_driver(request);
	});

	server.on("/deleteModbusDriver", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_delete_modbus_driver(request);
	});

	server.on("/updateModbusDriver", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_update_modbus_driver(request);
	});

	server.on("/getActiveModbusDriver", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_get_active_modbus_driver(request);
	});

	server.on("/useModbusDriver", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_use_modbus_driver(request);
	});

	server.on("/factoryReset", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_factory_reset(request);
	});

	server.on("/findDevice", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_find_device(request);
	});

	server.on("/getDeviceId", HTTP_POST, [](AsyncWebServerRequest *request) {
		server_get_device_id(request);
	});

	server.onNotFound([](AsyncWebServerRequest *request) {
		if (!server_handle_file_read(request))
		{
			request->send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
		}
		else
		{
			// Narayan - change this, put this function separately...
		}
	});

	server.begin();
}

bool server_handle_file_read(AsyncWebServerRequest *request)
{ // send the right file to the client (if it exists)
	String path = request->url().c_str();

	if (path.endsWith("/"))
		path += "views/index.html";				 // If a folder is requested, send the index file
	String contentType = get_content_type(path); // Get the MIME type
	String pathWithGz = path + ".gz";
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
	{ // If the file exists, either as a compressed archive, or normal

		if (SPIFFS.exists(pathWithGz))
		{				   // If there's a compressed version available
			path += ".gz"; // Use the compressed version

			AsyncWebServerResponse *response = request->beginResponse(SPIFFS, path, contentType, false);
			if (response != NULL)
			{
				response->addHeader("Content-encoding", "gzip");
				file_send_inprogress = true;
				request->send(response);
				file_send_inprogress = false;
			}
		}
		else
		{
			g_pLogger->Write(LogLevel::Info, "server_handle_file_read", "Sent file : %s", path.c_str());
			// delay(10);
			AsyncWebServerResponse *response = request->beginResponse(SPIFFS, path, contentType, false);
			if (response != NULL)
			{
				file_send_inprogress = true;
				request->send(response);
				file_send_inprogress = false;
			}
		}

		return true;
	}
	else
	{
		g_pLogger->Write(LogLevel::Error, "server_handle_file_read", "File Not Found: %s", path.c_str());
		return false; // If the file doesn't exist, return false
	}
}

String get_content_type(String filename)
{
	if (filename.endsWith(".htm"))
		return "text/html";
	else if (filename.endsWith(".html"))
		return "text/html";
	else if (filename.endsWith(".css"))
		return "text/css";
	else if (filename.endsWith(".js"))
		return "application/javascript";
	else if (filename.endsWith(".png"))
		return "image/png";
	else if (filename.endsWith(".gif"))
		return "image/gif";
	else if (filename.endsWith(".jpg"))
		return "image/jpeg";
	else if (filename.endsWith(".ico"))
		return "image/x-icon";
	else if (filename.endsWith(".xml"))
		return "text/xml";
	else if (filename.endsWith(".pdf"))
		return "application/x-pdf";
	else if (filename.endsWith(".zip"))
		return "application/x-zip";
	else if (filename.endsWith(".gz"))
		return "application/x-gzip";
	return "text/plain";
}

// This function funtions returns true  in AP mode
bool check_mode()
{
	// Narayan - remove this line in production...
	// return true;
	if (WiFi.getMode() == WIFI_MODE_AP)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void parse_bytes(const char *str, char sep, byte *bytes, int maxBytes, int base)
{
	for (int i = 0; i < maxBytes; i++)
	{
		bytes[i] = strtoul(str, NULL, base); // Convert byte
		str = strchr(str, sep);				 // Find next separator
		if (str == NULL || *str == '\0')
		{
			break; // No more separators, exit
		}
		str++; // Point to next character after separator
	}
}

// This handling root file 
void server_handle_root(AsyncWebServerRequest *request)
{
	String path = "/views/index.html";
	String contentType = "text/html";

	if (SPIFFS.exists(path))
	{
		file_send_inprogress = true;
		request->send(SPIFFS, path, contentType);
		file_send_inprogress = false;
	}
	else
	{
		request->send(200, "text/plain", "index.html not found...");
	}
}

void server_update_mqtt_config(AsyncWebServerRequest *request)
{
	// allocate the memory for the document
	const size_t CAPACITY = JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> doc;
	String response;
	String prtStr = "";

	// create an object
	JsonObject retVal = doc.to<JsonObject>();
	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	// Check if all arguments are presents,
	// else return...
	if (request->hasArg("mqtt"))
	{
		g_pLogger->Write(LogLevel::Info, "server_update_mqtt_config", "mqtt: %s", request->arg("mqtt").c_str());
	}
	else
	{
		retVal["result"] = 0;
		retVal["message"] = "Invalid parameters....";

		serializeJson(retVal, response);
		request->send(200, "text/json", response);
		return;
	}

	// if request from station mode, do not configure...
	if (check_mode() == true)
	{
		File file = SPIFFS.open("/config/mqtt.json", "w");

		if (!file)
		{
			//Narayan - Handle non availability of said file here...
			retVal["result"] = 0;
			retVal["message"] = "Error opening file for writing";
		}

		int bytesWritten = file.print(request->arg("mqtt"));
		file.close();

		if (bytesWritten > 0)
		{
			retVal["result"] = 1;
			retVal["message"] = "MQTT - Configuration Saved...";
		}
		else
		{
			retVal["result"] = 0;
			retVal["message"] = "MQTT - File write failed...";
		}
	}
	else
	{
		retVal["result"] = 0;
		retVal["message"] = "Unable to configure in station mode....";
	}

	serializeJson(retVal, response);
	g_pLogger->Write(LogLevel::Info, "server_update_mqtt_config", "response: %s", response.c_str());

	request->send(200, "text/json", response);
}

void server_update_Wifi_config(AsyncWebServerRequest *request)
{
	// allocate the memory for the document
	const size_t CAPACITY = JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> doc;
	String response;
	String prtStr = "";

	// create an object
	JsonObject retVal = doc.to<JsonObject>();
	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	// Check if all arguments are presents,
	// else return...
	if (request->hasArg("wifi_config"))
	{
		g_pLogger->Write(LogLevel::Info, "server_update_Wifi_config", "wifi_config: %s", request->arg("wifi_config").c_str());
	}
	else
	{
		retVal["result"] = 0;
		retVal["message"] = "Invalid parameters....";

		serializeJson(retVal, response);
		request->send(200, "text/json", response);
		return;
	}

	// if request from station mode, do not configure...
	if (check_mode() == true)
	{
		File file = SPIFFS.open("/config/wifi.json", "w");

		if (!file)
		{
			//Narayan - Handle non availability of said file here...
			retVal["result"] = 0;
			retVal["message"] = "Wifi - Error opening file for writing";
		}

		int bytesWritten = file.print(request->arg("wifi_config"));

		if (bytesWritten > 0)
		{
			retVal["result"] = 1;
			retVal["message"] = "Wifi configuration Saved...";
		}
		else
		{
			retVal["result"] = 1;
			retVal["message"] = "Wifi - File write failed...";
		}
		file.close();
	}
	else
	{
		retVal["result"] = 1;
		retVal["message"] = "Unable to configure in station mode...";
	}

	serializeJson(retVal, response);
	g_pLogger->Write(LogLevel::Info, "server_update_Wifi_config", "response: %s", request->arg("wifi_config").c_str());

	request->send(200, "text/json", response);
}

void server_get_device_config(AsyncWebServerRequest *request)
{
	String deviceConfig;
	serializeJson(device_doc, deviceConfig);
	request->send(200, "text/json", deviceConfig);
}

void server_get_sensor_values(AsyncWebServerRequest *request)
{
	// Narayan check if values are getting updated when this is accessed by server...
	String sensorValues;
	serializeJson(sensorValues_doc, sensorValues);

	request->send(200, "text/json", sensorValues);
}

void server_reset_device(AsyncWebServerRequest *request)
{
	// allocate the memory for the document
	const size_t CAPACITY = JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> doc;
	String response;

	// create an object
	JsonObject retVal = doc.to<JsonObject>();
	retVal["result"] = 0;
	retVal["message"] = "Device is being restarted..";

	serializeJson(retVal, response);
	request->send(200, "text/json", response);

	g_pLogger->Write(LogLevel::Warn, "server_reset_device", "Device is being restarted by client..");

	params.drc++;
	meti_nvs_commit_val("params", "drc", params.drc);

	//user restart from front end, restart in station mode...
	meti_nvs_commit_val("params", "forced_ap", FORCED_AP_NVS_FALSE);

	//restart device
	meti_device_restart();
}

void server_factory_reset(AsyncWebServerRequest *request)
{
	// allocate the memory for the document
	const size_t CAPACITY = JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> doc;
	String response;

	// create an object
	JsonObject retVal = doc.to<JsonObject>();
	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	// Check if all arguments are presents,
	// else return...
	if (request->hasArg("dev_key"))
	{
		// g_pLogger->Write(LogLevel::Info, "server_factory_reset", "mqtt: %s", request->arg("mqtt").c_str());

		// No need to set it as false and device will reset....
		file_send_inprogress = true;

		// Remove all files from /drivers
		File root = SPIFFS.open("/drivers");
		File file = root.openNextFile();

		while (file)
		{
			String fName = file.name();
			g_pLogger->Write(LogLevel::Info, "server_factory_reset", "Removing driver file: %s", fName.c_str());
			// Remove selected file...
			SPIFFS.remove(fName);
			file = root.openNextFile();
		}

		file.close();
		root.close();

		// Remove all files from /config
		root = SPIFFS.open("/config");
		file = root.openNextFile();
		while (file)
		{
			String fName = file.name();
			g_pLogger->Write(LogLevel::Info, "server_factory_reset", "Removing config file: %s", fName.c_str());
			// Remove selected file...
			SPIFFS.remove(fName);
			file = root.openNextFile();
		}

		file.close();
		root.close();

		g_pLogger->Write(LogLevel::Info, "server_factory_reset", "Copying config files");

		//copy all files from faConfig to config
		String srcDir = "/faConfig";
		String fName;
		String fwName;
		String writeString;

		root = SPIFFS.open(srcDir);
		File fileRead = root.openNextFile();
		File fileWrite;
		while (fileRead)
		{
			fName = fileRead.name();
			fwName = "/config" + fName.substring(srcDir.length(), 25);
			g_pLogger->Write(LogLevel::Info, "server_factory_reset", "Copying config file: %s to %s", fName.c_str(), fwName.c_str());
			fileWrite = SPIFFS.open(fwName, "w");

			writeString = "";
			// Transfer contents from source to destination...
			while (fileRead.available())
			{
				writeString += char(fileRead.read());
			}

			fileWrite.print(writeString);

			fileWrite.close();
			fileRead = root.openNextFile();
		}

		fileRead.close();
		root.close();

		retVal["result"] = 1;
		retVal["message"] = "Device is being restarted..";

		serializeJson(retVal, response);
		request->send(200, "text/json", response);

		if (retVal["result"] == 1)
		{
			delay(1000);
			meti_device_restart();
		}
	}
}

void server_update_modbus_driver(AsyncWebServerRequest *request)
{
	// allocate the memory for the document
	const size_t CAPACITY = JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> doc;
	String response;

	// create an object
	JsonObject retVal = doc.to<JsonObject>();
	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	// Check if all arguments are presents,
	// else return...
	if ((request->hasArg("modbus_driver")) && (request->hasArg("id")))
	{
		Serial.printf("modbus_driver: %s\n", request->arg("modbus_driver").c_str());
		Serial.printf("id: %s\n", request->arg("id").c_str());
	}
	else
	{
		retVal["result"] = 0;
		retVal["message"] = "Invalid parameters....";

		serializeJson(retVal, response);
		request->send(200, "text/json", response);
		return;
	}

	// works in both station and ap mode

	String fName = "/drivers/mmd_";
	fName = fName + request->arg("id");
	fName = fName + ".json";

	if (!SPIFFS.exists(fName))
	{
		//Narayan - Handle non availability of said file here...
		retVal["result"] = 0;
		retVal["message"] = "File does not exist...";

		serializeJson(retVal, response);
		request->send(200, "text/json", response);
		return;
	}

	// if exists open file to write
	File file = SPIFFS.open(fName, "w");

	if (!file)
	{
		//Narayan - Handle non availability of said file here...
		retVal["result"] = 0;
		retVal["message"] = "Error opening driver file for writing";
	}

	int bytesWritten = file.print(request->arg("modbus_driver"));

	if (bytesWritten > 0)
	{
		retVal["result"] = 1;
		retVal["message"] = "Sensor updation succesful...";
	}
	else
	{
		retVal["result"] = 0;
		retVal["message"] = "Sensor update failed...";
	}
	file.close();

	serializeJson(retVal, response);
	request->send(200, "text/json", response);
}

void server_save_new_modbus_driver(AsyncWebServerRequest *request)
{
	// allocate the memory for the document
	const size_t CAPACITY = JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> doc;
	String response;

	// create an object
	JsonObject retVal = doc.to<JsonObject>();
	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	// Check if all arguments are presents,
	if ((request->hasArg("driver_id")) && (request->hasArg("driver_file0")) && (request->hasArg("driver_chunks")) && (request->hasArg("fileSize")))
	{
		char chunck_name[32];
		uint8_t chunks = request->arg("driver_chunks").toInt();
		uint16_t fs = 0;
		for (size_t i = 0; i < chunks; i++)
		{
			sprintf(chunck_name, "driver_file%d", i);
			fs += request->arg(chunck_name).length();
		}

		uint16_t fileSize = request->arg("fileSize").toInt();

		g_pLogger->Write(LogLevel::Debug, "save_mmd", "fileSize: %d, fs:%d", fileSize, fs);

		if (fileSize == fs)
		{
			// String fName = "/" + "test.html";
			String fName = "/drivers/mmd_";
			fName = fName + request->arg("driver_id");
			fName = fName + ".json";

			// if exists open file to write
			File file = SPIFFS.open(fName, "w");

			if (!file)
			{
				//Narayan - Handle non availability of said file here...
				retVal["message"] = "Error opening driver file for writing";
			}
			else
			{
				file_send_inprogress = true;

				int bytesWritten = 0;
				// int chunks = request->arg("driver_chunks").toInt();

				// char chunck_name[32];
				for (size_t i = 0; i < chunks; i++)
				{
					sprintf(chunck_name, "driver_file%d", i);
					bytesWritten += file.print(request->arg(chunck_name));
					// g_pLogger->Write(LogLevel::Debug, "save_mmd", "file: %s  data:%s", chunck_name, request->arg(chunck_name).c_str());
				}

				file_send_inprogress = false;

				if (bytesWritten > 0)
				{
					retVal["result"] = 1;
					retVal["message"] = "Driver save succesful...";
				}
				else
				{
					retVal["message"] = "Driver save failed...";
				}
			}
			file.close();
		}
		else
		{
			retVal["message"] = "Unable to update in station mode...";
		}
	}
	else
	{
		retVal["message"] = "File size mismatch... Save again...";
	}

	serializeJson(retVal, response);
	g_pLogger->Write(LogLevel::Debug, "save_mmd", "response: %s", response.c_str());
	request->send(200, "text/json", response);
}

void server_use_modbus_driver(AsyncWebServerRequest *request)
{
	String str;
	// allocate the memory for the document
	const size_t CAPACITY = JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> doc;
	String response;

	// create an object
	JsonObject retVal = doc.to<JsonObject>();
	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	// Check if all arguments are presents,
	// else return...
	if ((request->hasArg("driver_id")) && (request->hasArg("active")))
	{

		DynamicJsonDocument mmd_doc(modbusDriverSize);

		// String fName = "/" + "test.html";
		String fName = "/drivers/mmd_";
		fName = fName + request->arg("driver_id");
		fName = fName + ".json";

		// if exists open file to write
		File file = SPIFFS.open(fName, "r");

		if (!file)
		{
			//Narayan - Handle non availability of said file here...
			retVal["result"] = 0;
			retVal["message"] = "Error opening driver file for writing";
		}
		else
		{
			deserializeJson(mmd_doc, file);
			file.close();

			if (mmd_doc.containsKey("driver_id"))
			{
				g_pLogger->Write(LogLevel::Debug, "save_mmd", "response: %s", request->arg("active"));
				if (request->arg("active") == "true")
				{
					mmd_doc["active"] = true;
				}
				else
				{
					mmd_doc["active"] = false;
				}

				serializeJson(mmd_doc, str);

				file = SPIFFS.open(fName, "w");
				int bytesWritten = file.print(str);
				file.close();

				retVal["result"] = 1;
				retVal["message"] = "Changes applied....";
				// Handle file write fail here...
			}
			else
			{
				retVal["result"] = 0;
				retVal["message"] = "Invalid Driver file...";
			}
		}
	}
	else
	{
		retVal["result"] = 0;
		retVal["message"] = "Invalid parameters....";
	}

	serializeJson(retVal, response);
	g_pLogger->Write(LogLevel::Debug, "save_mmd", "response: %s", response.c_str());
	request->send(200, "text/json", response);
}

void server_delete_modbus_driver(AsyncWebServerRequest *request)
{
	// allocate the memory for the document
	const size_t CAPACITY = JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> doc;
	String response;

	// create an object
	JsonObject retVal = doc.to<JsonObject>();

	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	if ((request->hasArg("driver_id")) && (request->hasArg("app_key")))
	{
		// if request from station mode, do not configure...
		String fName = "/drivers/mmd_";
		fName = fName + request->arg("driver_id");
		fName = fName + ".json";

		//Delete the file from FS
		SPIFFS.remove(fName);

		retVal["result"] = 1;
		retVal["message"] = "Driver removed from device...";
	}
	else
	{
		retVal["result"] = 0;
		retVal["message"] = "Invalid parameters....";
	}

	serializeJson(retVal, response);
	g_pLogger->Write(LogLevel::Debug, "get_mmds", "response: %s", response.c_str());
	request->send(200, "text/json", response);
}

void server_get_all_modbus_driver(AsyncWebServerRequest *request)
{
	const size_t CAPACITY = JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> doc;
	String response;

	DynamicJsonDocument mmd_doc(modbusDriverSize);

	const size_t inner_obj_capacity = JSON_OBJECT_SIZE(13) + 100;
	StaticJsonDocument<inner_obj_capacity> inner_doc;
	StaticJsonDocument<inner_obj_capacity * 32> outer_doc;
	// JsonArray outer_array = outer_doc.to<JsonArray>();
	outer_doc["result"] = 1;
	JsonArray outer_array = outer_doc.createNestedArray("driver_list");

	// create an object
	JsonObject retVal = doc.to<JsonObject>();
	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	// Check if all arguments are presents,
	// else return...
	if (request->hasArg("app_key"))
	{
		// Serial.printf("driver_file: %s\n", request->arg("driver_file").c_str());
		// Serial.printf("id: %s\n", request->arg("driver_id").c_str());
	}
	else
	{
		retVal["result"] = 0;
		retVal["message"] = "Invalid parameters....";

		serializeJson(retVal, response);
		g_pLogger->Write(LogLevel::Debug, "get_mmds", "response: %s", response.c_str());

		request->send(200, "text/json", response);
		return;
	}

	File root = SPIFFS.open("/drivers");
	File file = root.openNextFile();

	while (file)
	{
		String fName = file.name();
		// g_pLogger->Write(LogLevel::Debug, "get_mmds", "file:%s", fName.substring(0, 12).c_str());
		if (fName.substring(0, 13) == "/drivers/mmd_")
		{
			deserializeJson(mmd_doc, file);

			if (mmd_doc.containsKey("driver_id"))
			{
				inner_doc["driver_id"] = mmd_doc["driver_id"];
				inner_doc["vendor"] = mmd_doc["vendor"];
				inner_doc["meter"] = mmd_doc["meter"];
				inner_doc["name"] = mmd_doc["name"];
				inner_doc["baud"] = mmd_doc["serial"]["baud"];
				inner_doc["parity"] = mmd_doc["serial"]["parity"];
				inner_doc["stopbits"] = mmd_doc["serial"]["stopbits"];
				inner_doc["slaveId"] = mmd_doc["command"]["slaveId"];
				inner_doc["startAddr"] = mmd_doc["command"]["startAddr"];
				inner_doc["regCount"] = mmd_doc["command"]["regCount"];
				inner_doc["repeat_int"] = mmd_doc["command"]["repeat_int"];
				inner_doc["active"] = mmd_doc["active"];
				inner_doc["params"] = " ";

				outer_array.add(inner_doc);
				inner_doc.clear();
			}
		}
		file = root.openNextFile();
	}
	file.close();
	root.close();

	serializeJson(outer_doc, response);

	g_pLogger->Write(LogLevel::Debug, "get_mmds", "response: %s", response.c_str());
	request->send(200, "text/json", response);
}

void server_get_active_modbus_driver(AsyncWebServerRequest *request)
{
	// allocate the memory for the document
	const size_t CAPACITY = JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> doc;
	String response;

	DynamicJsonDocument mmd_doc(modbusDriverSize);

	// create an object
	JsonObject retVal = doc.to<JsonObject>();

	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	if ((request->hasArg("app_key")))
	{
		if (active_driver_id > 0)
		{
			// if request from station mode, do not configure...
			String fName = "/drivers/mmd_";
			fName = fName + active_driver_id;
			fName = fName + ".json";

			AsyncWebServerResponse *response = request->beginResponse(SPIFFS, fName, "text/json", false);
			if (response != NULL)
			{
				retVal["result"] = 1;
				file_send_inprogress = true;
				request->send(response);
				file_send_inprogress = false;
			}
			else
			{
				retVal["message"] = "Unable to send driver file....";
			}
		}
		else
		{
			retVal["message"] = "No active Modbus driver/s....";
		}
	}
	else
	{
		retVal["message"] = "Invalid parameters....";
	}

	if (retVal["result"] == 0)
	{
		serializeJson(retVal, response);
		g_pLogger->Write(LogLevel::Debug, "get_mmds", "response: %s", response.c_str());
		request->send(200, "text/json", response);
	}
}

void server_find_device(AsyncWebServerRequest *request)
{
	const size_t CAPACITY = JSON_OBJECT_SIZE(5);
	StaticJsonDocument<CAPACITY> doc;
	String response;

	// create an object
	JsonObject retVal = doc.to<JsonObject>();

	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	if ((request->hasArg("app_key")))
	{
		if (request->arg("app_key").equals("quickIoT"))
		{
			String fName = "/config/device.json";

			AsyncWebServerResponse *response = request->beginResponse(SPIFFS, fName, "text/json", false);
			if (response != NULL)
			{
				retVal["result"] = 1;
				file_send_inprogress = true;
				request->send(response);
				file_send_inprogress = false;
			}
			else
			{
				retVal["message"] = "Unable to send driver file....";
			}
		}
	}

	if (retVal["result"] == 0)
	{
		serializeJson(retVal, response);
		g_pLogger->Write(LogLevel::Debug, "get_mmds", "response: %s", response.c_str());
		request->send(200, "text/json", response);
	}
}

void server_get_device_id(AsyncWebServerRequest *request)
{
	const size_t CAPACITY = JSON_OBJECT_SIZE(3);
	StaticJsonDocument<CAPACITY> doc;
	String response;

	// create an object
	JsonObject retVal = doc.to<JsonObject>();

	retVal["result"] = 0;
	retVal["message"] = "Unknown response...";

	if ((request->hasArg("app_key")))
	{
		if (request->arg("app_key").equals("quickIoT"))
		{
			retVal["result"] = 1;
			retVal["message"] = "Response ok...";

			const char* dev_id = device_doc["dev_id"];

			retVal["dev_id"] = dev_id;
		}
	}

	serializeJson(retVal, response);
	g_pLogger->Write(LogLevel::Debug, "get_dev_id", "response: %s", response.c_str());
	request->send(200, "text/json", response);
}*/